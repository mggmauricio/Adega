import pandas as pd
from control import *
def main():
    df = pd.read_csv('Planta.csv') #Clicando em "Arquivos na lateral esquerda
    # do colab, você pode fazer upload de arquivos. Faça o upload do arquivo
    # de dados dentro do diretório myenv"
    tau = 140.6 #Tempo que leva para a FT chegar a 63.2 % do seu valor final
    a = 1/tau
    RP = 8.9 #Valor da temperatura em regime permanente
    K = RP*a
    n  = [K]
    d = [1,a]
    G = tf(n,d)
    order = 3
    delay = 48
    num, den = pade(delay, order)
    Gdelay = tf(num,den)
    G = G*Gdelay
    print("G(s): ", G)
    figure(figsize=(12, 6), dpi=80)
    plt.figure(1)
    mag, phase, om = bode(G, logspace(-2, 1), plot=True)
    plt.show(block=True)
    figure(figsize=(12, 6), dpi=80)
    yout, T = step(G)
    plt.grid()
    plt.title('Step Response')
    plt.xlabel('Time (s)')
    plt.ylabel('Temperature (ºC)')
    plt.plot(T.T, yout.T)
    planta = df
    tempo = df['Tempo']
    temp = df['Temperatura']
    planta =planta[0:len(T.T)]
    plt.plot(tempo, temp)
    plt.legend(['G(s) obtida', 'Experimental'])
    plt.show(block=True)
    print(temp)


main()