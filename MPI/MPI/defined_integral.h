#pragma once

typedef double (*IntegralFunction)(double);
enum class IntegralType { Simpson, CentralRectangles, Trapezium };

/*
    func - функция для которой будет посчитан интеграл
    from - ночало отрезка
    to   - конец отрезка
    step - шаг
    type - метод для подсчета интеграла
*/
double DefiniteIntegral(IntegralFunction func,
                        double from,
                        double to,
                        double step = 0.1,
                        IntegralType type = IntegralType::Simpson);