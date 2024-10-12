# arduino-micro-console
Небольшой прототип микро-консоли.</br>
Игры внутри:
1. Arkanoid
2. Shooter
3. Jump

## Компоненты
1. Матрица 8x8 (MAX7219) 
2. Три кнопки и три резистора ~10 кОм

## Сборка
```
Распиновка матрицы:
  VCC - 5v
  GND - Gnd
  DIN - D11
  CS  - D5
  CLK - D13

Кнопки:
  Левая D2
  Правая D3
  Средняя D4
```

## Управление
Для выхода из игры в меню зажать все три кнопки на 1 секунду.
