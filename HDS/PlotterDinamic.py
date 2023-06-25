# IMPORTAÇÕES IMPORTANTES
import dearpygui.dearpygui  as dpg
import sympy as sp
import sys 
import os 

#SAVE func TXT
def save_txt( value : str, name : str = 'funcDoc', replace : bool = True):
    if replace: 
        with open( name + '.txt', 'w' ) as f:
            f.write( value )
    else: 
        with open( name + '.txt', 'a' ) as f: 
            f.write( value  + '\n' ) 

def read_txt( name = 'funcDoc') -> str: 
    with open( name + '.txt', 'r') as f:
        return f.readline()


FILE_NAME = sys.argv[0]
PATH = os.path.dirname( __file__ ).removesuffix( FILE_NAME ) + '\\'

t = sp.Symbol( 't' )
s = sp.Symbol( 's' ) 

lastFunctionInput = read_txt()
func = sp.lambdify(t, lastFunctionInput, 'numpy' )

# Valores das séries
XSerie = []
YSerie = [] 
XGsSerie = []
YGsSerie = [] 

# CONVOLUÇÃO 
def conv( x : list, y : list, num : int ):
    DOM = x
    VAL = [] 
    for i in range( len( y ) - num ): 
        SUM = 0 
        for j in y[ i : i+num ]:
            SUM += j 
        VAL.append( SUM/num )

    for i in range( num, 0, -1 ):
        SUM = 0
        for j in y[ -i : ]: 
            SUM += j 
        VAL.append( SUM/i )
    return [ DOM, VAL ]

# LEITOR DE TXT
def read_txt( name : str ):
    VALUES = [] 
    DOMAIN = []
    with open( PATH + name, 'r' ) as FILE: 
        count = 0 
        offset = float(FILE.readline().split(' ')[0])
        FILE.seek(0)
        for line in FILE.readlines():
            line = line.split( ' ' )
            value = float(line[0]) - offset
            VALUES.append( value )
            DOMAIN.append( count )
            count += 2
    return [ DOMAIN, VALUES ]



def L( f, t, s):
    return sp.laplace_transform(f, t, s )

def invL(f, t, s):
    return sp.inverse_laplace_transform( f, s, t) 


# Leitura dos dados 
DOM10, VAL10 = read_txt( 'Caminha2.txt' )
# Convolução dos dodos
DOM10, VAL10 = conv( DOM10, VAL10, 10 )

XSerie = DOM10 
YSerie = VAL10 
YSerieMin = min(VAL10)*1.1 if  min(VAL10) < 0 else  min(VAL10) * 0.9 
YSerieMax = max(VAL10)*1.1

tal = 1/750
r1  = 3/750
r2  = 6/750
K   = 30/25


# INICIO DO CONTEXTO DO DPG 
dpg.create_context()
dpg.create_viewport( title = 'Controle do brabo Micael Shumas - Rei do filtro de Kalman', min_width = 800, min_height = 600 )
dpg.setup_dearpygui()



def compute_function( sender, data, user ): 
    lastFunctionInput = data
    save_txt( data )
    func = sp.lambdify(t, lastFunctionInput, 'numpy' )
    XGsSerie = []
    YGsSerie = []
    for x in XSerie:
        XGsSerie.append( x )
        YGsSerie.append( func(x) )
    dpg.configure_item( 'G(s)', x = XGsSerie, y = YGsSerie )

# FUNÇÃO
def init_main():
    with dpg.window( label = 'Main Window', tag = 'mainWindow', autosize = True ):
        
        with dpg.window( tag = 'plotter_window', label = 'Ploter function', width =1000, height = 800, pos = [10,10]  , no_move = True, no_resize = True ):
            with dpg.plot( tag = 'ploter', label = 'Temperatura e G(s)', height = -1, width = -1, anti_aliased = True ): 
                dpg.add_plot_legend( )
                with dpg.plot_axis      ( dpg.mvXAxis, tag = 'x_axis_ploter', label = 'time [s]' ):
                    dpg.set_axis_limits ( axis = 'x_axis_ploter', ymin = 0, ymax = XSerie[-1]+1000 )


                with dpg.plot_axis      ( dpg.mvYAxis, tag = 'y_axis_ploter', label = 'Angulo [º]' ):                    
                    dpg.set_axis_limits ( axis = 'y_axis_ploter', ymin = YSerieMin - 10, ymax = YSerieMax + 20)

                    dpg.add_line_series ( x = XSerie, y = YSerie, tag = 'Temperatura', label = 'Temperatura' )
                    dpg.add_line_series ( x = XGsSerie, y = YGsSerie, tag = 'G(s)', label = 'Funcao G(s)' ) 
        
        with dpg.window( tag = 'config', label = 'Configurações de função', width = 445, height = 800, pos = [1020,10], no_move = True, no_resize = True  ):
            
            dpg.add_text( 'Parametros da função lida')
            dpg.add_input_text( default_value = max(YSerie), label = 'Vmax', width = 300 )
            dpg.add_slider_float( width = 300, label = 'r1')
            
            dpg.add_text( 'Digite a função para testar')
            dpg.add_input_text( tag = 'func', default_value = lastFunctionInput, callback = compute_function, on_enter = True, width = -1 )
            
            dpg.add_separator( )
            dpg.add_text( 'G(s)')

# CONSTRUÇÃO DO ESCOPO MAIN 
init_main() 

# CONFIGURAÇÕES 
dpg.set_primary_window          ( 'mainWindow', True                    )
dpg.maximize_viewport           (                                       ) 

# INICIAR AS VIEWS DO DPG
dpg.show_viewport( )

# LAÇO DE RENDERIZAÇÃO DO DPG
while dpg.is_dearpygui_running():
    dpg.render_dearpygui_frame() 

