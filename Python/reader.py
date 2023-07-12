import serial
import csv
import time
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import threading

graphname = f'grafico_estufa{datetime.now()}.png'
dataname = f'data_estufa{datetime.now()}.csv'
nAmostras = 2100
# Configuração da porta serial
serial_port = '/dev/ttyUSB1'  # Substitua pela porta serial correta
baud_rate = 9600

# Inicializa a conexão serial
ser = serial.Serial(serial_port, baud_rate)

# Cria um arquivo CSV vazio e escreve o cabeçalho
with open(dataname, 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Tempo', 'Temperatura', 'Amostra', 'setpoint', 'duty', 'y_controller', 'pwm'])

time_ = []
temperature = []
setpoint_list = []
duty_list = []
y_controller_list = []
pwm_list = []
start_time = datetime.now()

fig, ax = plt.subplots()
line, = ax.plot([], [])
ax.set_xlabel('Tempo (s)')
ax.set_ylabel('Temperatura')


def update_graph(frame):
    line.set_data(time_, temperature)
    ax.relim()
    ax.autoscale_view()
    
    # Salva o gráfico em uma imagem
    fig.savefig(graphname)


def read_serial_data():
    i = 0
    with open(dataname, 'a', newline='') as file:
        writer = csv.writer(file)
        while True:
            # Lê uma linha da porta serial
            data = ser.readline().decode().strip().split(',') 

            if len(data) == 5:
                print(data[0])
                current_temperature = float(data[0])
                print(data[0])
                setpoint = float(data[1])
                duty = int(data[2])
                y_controller = float(data[3])
                pwm = int(data[4])

                current_time = datetime.now()
                elapsed_time = (current_time - start_time).total_seconds()
                elapsed_time = round(elapsed_time, 2)

                try:
                    # Adiciona os dados às listas
                    time_.append(elapsed_time)
                    temperature.append(current_temperature)
                    setpoint_list.append(setpoint)
                    duty_list.append(duty)
                    y_controller_list.append(y_controller)
                    pwm_list.append(pwm)

                    print(f'Tempo: {elapsed_time} segundos, Temperatura: {current_temperature} Amostra:{i}, setpoint: {setpoint}, duty: {duty}, y_controller : {y_controller}, pwm: {pwm}')

                    # Escreve os dados diretamente no arquivo CSV
                    writer.writerow([elapsed_time, current_temperature, i, setpoint, duty, y_controller, pwm])

                    i += 1
                    if i >= nAmostras:
                        break
                except:
                    pass


# Chama a função de leitura dos dados serial em um thread separado
serial_thread = threading.Thread(target=read_serial_data)
serial_thread.start()

# Atualiza o gráfico em tempo real com a função de animação
ani = FuncAnimation(fig, update_graph, interval=1000)
plt.show()

# Aguarda a conclusão da leitura dos dados serial
serial_thread.join()
