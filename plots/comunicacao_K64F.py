import serial, time
import pandas as pd



def main():
	time_step = 1 #1 segundo por aquisição
	num_iterations = 2000/time_step
	it = 0
	tempo = []
	temperatura = []
	duty = []
	while it < num_iterations:
		conn = serial.Serial('COM3', 115200, timeout=3)
		time.sleep(0.6)
		try:
			leitura = conn.readline()
			print("Leitura: ", leitura)
			leitura = str(leitura)
			valores = leitura.split(',')
			valores[0] = valores[0][2:len(valores[0])]
			valores[2] = valores[2][0:len(valores[2])-3]
			print("Leitura separada: ", valores)
			tempo.append(int(valores[0]))
			temperatura.append(int(valores[1])/100)
			duty.append(int(valores[2]))
		except IndexError:
			tempo.append(0)
			temperatura.append(0)
			duty.append(0)
				
		conn.close()
		it += 1
	
	df = pd.DataFrame({'tempo' : tempo, 'temperatura' : temperatura, 'duty' : duty})
	print(df)
	df.to_csv('Output.csv')


main()

