На лекциях вы познакомились с умным указателем shared_ptr, который обеспечивает совместное владение объектом. В данной задаче вам нужно будет реализовать классы фигур, которые можно отрисовывать на изображении, и которые совместно владеют заданной текстурой.

Среди доступных вам файлов вас в первую очередь будет интересовать файл Common.h. В нём приведён интерфейс IShape, который описывает фигуру. Существуют два вида фигур: прямоугольник и эллипс. Они указаны в перечислении ShapeType. Ваша задача реализовать функцию MakeShape, которая создаёт фигуру заданного вида.

Интерфейс IShape позволяет запрашивать и задавать ряд свойств фигуры: позицию, размер и текстуру. Также он позволяет клонировать фигуру с помощью метода Clone(). Это похоже на то, как если бы вы выбрали в редакторе графики фигуру и нажали Ctrl+C и Ctrl+V. Идея в том, что склонированная фигура использует ту же самую текстуру, что и оригинал. Это удобно, т.к. в общем случае текстура может занимать много памяти. Разумеется, в дальнейшем склонированной фигуре можно задать новую текстуру.

Объект текстуры должен быть удалён в тот момент, когда нет больше фигур, которые имеют данную текстуру. Это может произойти при удалении фигуры, или при назначении фигуре другой текстуры.

Кроме того, интерфейс IShape позволяет отрисовать текущую фигуру на заданном изображении с помощью метода Draw(). Отрисовка происходит по следующим правилам: