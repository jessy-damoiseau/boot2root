import turtle

contents = open("turtle", "r").readlines()

turtle.speed(0)

turtle.penup()
turtle.goto(0, 0)
turtle.pendown()

for line in contents:
    l = line.strip()
    if "Avance" in l:
        val = int(l.split()[1])
        turtle.forward(val)
    elif "Recule" in l:
        val = int(l.split()[1])
        turtle.backward(val)
    elif "Tourne droite" in l:
        val = int(l.split()[3])
        turtle.right(val)
    elif "Tourne gauche" in l:
        val = int(l.split()[3])
        turtle.left(val)
    else:
        print(l)

turtle.done()