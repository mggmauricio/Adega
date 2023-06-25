# IMPORTAÇÕES IMPORTANTES 
from struct import unpack

import dearpygui.dearpygui  as dpg
import serial  
import os 

# INICIO DO CONTEXTO DO DPG 
dpg.create_context()
dpg.create_viewport( title = 'Controle por computador', min_width = 800, min_height = 600 )
dpg.setup_dearpygui()

# Valores das séries
XSerie = []
YSerie = [] 

# FUNÇÃO
def init_main():
    with dpg.window( label = 'Main Window', tag = 'mainWindow', autosize = True ):
        with dpg.plot( tag = 'ploter', label = 'Temperatura', height = -1, width = -1, anti_aliased = True ): 
            dpg.add_plot_legend( )
            with dpg.plot_axis      ( dpg.mvXAxis, tag = 'x_axis_ploter', label = 'time [s]' ):
                dpg.set_axis_limits ( axis = 'x_axis_ploter', ymin = 0, ymax = 100 )
            with dpg.plot_axis      ( dpg.mvYAxis, tag = 'y_axis_ploter', label = 'Angulo [º]' ):
                dpg.set_axis_limits ( axis = 'y_axis_ploter', ymin = 10, ymax = 100 )
                dpg.add_line_series ( x = XSerie, y = YSerie, tag = 'Temperatura', label = 'Temperatura' )


# CONSTRUÇÃO DO ESCOPO MAIN 
init_main() 

PATH = os.path.dirname( __file__ ).removesuffix( 'Medidor_temperatura.py')
COMP = serial.Serial( 'COM7', baudrate = 9600 )
medida = dTime = dTime = 0 

# CONFIGURAÇÕES 
dpg.set_primary_window          ( 'mainWindow', True                    )
dpg.maximize_viewport           (                                       ) 

# INICIAR AS VIEWS DO DPG
dpg.show_viewport( )

# LAÇO DE RENDERIZAÇÃO DO DPG
while dpg.is_dearpygui_running():
    dpg.render_dearpygui_frame() 
    
    # ATUALIZAÇÃO DOS FRAMES DOS PLOTES 
    if dpg.get_frame_count() % 10  == 0:
        if COMP.inWaiting() < 4:
            continue 
        else:
            Medida = unpack( '<f', COMP.read( 4 ) )[0]
            dTime += 2
            
            print( Medida, dTime )

            XSerie.append( dTime  ) 
            YSerie.append( Medida )
        
            with open(PATH + '\\Caminha.txt', 'a' ) as FILE:
                FILE.write( str(Medida) + ' ' + str(dTime) + '\n' )

            dpg.configure_item( 'Temperatura', x = XSerie, y = YSerie )
            dpg.set_axis_limits( 'x_axis_ploter', ymin = XSerie[0], ymax = XSerie[-1] )

