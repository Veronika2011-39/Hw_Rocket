
def get_data(path = ""):
    data = {}
    with open(path, 'r', encoding= "utf-8") as file:
        for i, line in enumerate(file): 
             data[i + 1] = [
                 int(i) if i.isdigit()
                 else float(i) 
                 for i in list(line.strip().split(";"))[1:-1]
                 ]
        return data 


#4578
  
def format_timeS(data, start = 1, stop = None, rounding_eror= 200):
    sumMS = 0.0 
    for i in range(start, stop if  stop else len(data)):
        print(f"Время: {round(sumMS/1000,2)} Высота:{data [i][1]} Давление: {data [i][2]}")
        if i + 1 in data:
            if abs(data[i][0]) - data[i+1][0] > rounding_eror:
                sumMS += 65
            else:
                 sumMS += abs(data[i][0] - data[i+1][0])
    
          



data = get_data("telemetry/23-е Небо.txt")   
data = format_timeS(data,start=4570)