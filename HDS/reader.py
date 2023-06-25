import serial
import pandas as pd
import time
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import threading

graphname = f'grafico{datetime.now()}.png'





nAmostras = 1000
# Configuração da porta serial
serial_port = '/dev/ttyUSB0'  # Substitua pela porta serial correta
baud_rate = 9600

# Inicializa a conexão serial
ser = serial.Serial(serial_port, baud_rate)

# Cria um DataFrame vazio para armazenar os dados
data = pd.DataFrame(columns=['Temperatura','Tempo'])
time_ = []
temperature = []
start_time = datetime.now()

# plt.ion()  # Modo de interação
# fig, ax = plt.subplots()
# line, = ax.plot([], [])
# ax.set_xlabel('Tempo (s)')
# ax.set_ylabel('Temperatura')
fig, ax = plt.subplots()
line, = ax.plot([], [])
ax.set_xlabel('Tempo (s)')
ax.set_ylabel('Temperatura')

def update_graph(frame):
    line.set_data(time_, temperature)
    ax.relim()
    ax.autoscale_view()
    
    # Salva o gráfico em uma imagem
    fig.savefig('grafico.png')

def update_graph(frame):
    line.set_data(time_, temperature)
    ax.relim()
    ax.autoscale_view()
    
    # Salva o gráfico em uma imagem
    fig.savefig(graphname)


def read_serial_data():
    i = 0
    while True:
        # Lê uma linha da porta serial
        l = ser.readline().decode().strip()
        msg = str(ser.readline())
        current_temperature = float(msg[2:-5])
        ser.flush()
        
        current_time = datetime.now()
        elapsed_time = (current_time - start_time).total_seconds()
        elapsed_time = round(elapsed_time, 2)
        

        # Adiciona os dados às listas
        time_.append(elapsed_time)
        temperature.append(float(current_temperature))
        
        
        print(f'Tempo: {elapsed_time} segundos, Temperatura: {float(current_temperature)} Amostra:{i}')
        
        # Adiciona os dados ao DataFrame
        data.append({'Tempo': elapsed_time, 'Temperatura': float(current_temperature)}, ignore_index=True)
        
        i += 1
        if i >= nAmostras:
            break

# Chama a função de leitura dos dados serial em um thread separado
serial_thread = threading.Thread(target=read_serial_data)
serial_thread.start()

# Atualiza o gráfico em tempo real com a função de animação
ani = FuncAnimation(fig, update_graph, interval=1000)
plt.show()

# Aguarda a conclusão da leitura dos dados serial
serial_thread.join()

# Salva o DataFrame no arquivo CSV
data.to_csv('dados.csv', index=False)
