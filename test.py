import mgfx
import time

mgfx.create_window("My Window", "MAIN_WINDOW", 600, 400)

#Игровой цикл, управляемый через ID окна
while mgfx.is_open("MAIN_WINDOW"):
    #Здесь в будущем будут вызовы рендеринга OpenGL

    #Обновляем окно по его ID
    mgfx.update("MAIN_WINDOW")

#Освобождаем ресурсы
mgfx.terminate()
print("Работа завершена.")