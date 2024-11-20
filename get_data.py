data = {}
with open('telemetry\\23-е Небо.txt', 'r', encoding= "utf-8") as file:
    for i, line in enumerate(file): 
        data[i + 1] = [int(i) if i.isdigit() else float(i) for i in list(line.strip().split(";"))[1:-1]]
       
       
       
print(data[1][0] + data[2][0])
