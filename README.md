# OSHomework_1
## Кузьмин Максим Михайлович, БПИ212
## Вариант 1
## Условие: Задача о парикмахере. В тихом городке есть парикмахерская.
Салон парикмахерской мал, работать в нем нем может только один
парикмахер, обслуживающий одного посетителя. Есть несколько кресел для ожидания в очереди. Парикмахер всю жизнь обслуживает посетителей. Когда в салоне никого нет, он спит в кресле.
Когда посетитель приходит и видит спящего парикмахера, он будет
его, садится в кресло, «засыпая» на тот момент, пока парикмахер
обслуживает его. Если посетитель приходит, а парикмахер занят,
то он встает в очередь и «засыпает». После стрижки парикмахер
сам провожает посетителя. Если есть ожидающие посетители, то
парикмахер будит одного из них и ждет пока тот сядет в кресло парикмахера и начинает стрижку. Если никого нет, он снова садится
в свое кресло и засыпает до прихода посетителя. Создать приложение, моделирующее рабочий день парикмахерской. Парикмахера и каждого из посетителей моделировать в виде отдельных процессов. Последние могут независимо порождаться в
произвольное время и завершать работу после обслуживания.
## На 4:
У нас есть общий процесс - парикмахер и дочерние процессы - клиенты. В общей памяти хранится информация о состоянии парикмахера (свободен/занят) и количество людей в очереди. Если людей в очереди нет, парикмахер ложится спать. Клиенты смотрят, если парикмахер занят, они ждут совей очереди, а если свободен, то начинается стрижка.
Семафоры применены следующие: Множество процессов взаимодействуют с использованием именованных POSIX семафоров. Обмен данными ведется через
разделяемую память в стандарте POSIX.
Удаление семафоры предусмотрено. 
Построена полная симуляция рабочего дня парикмахерской.
