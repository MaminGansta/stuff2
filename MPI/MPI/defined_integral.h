#pragma once

typedef double (*IntegralFunction)(double);
enum class IntegralType { Simpson, CentralRectangles, Trapezium };

/*
    func - ������� ��� ������� ����� �������� ��������
    from - ������ �������
    to   - ����� �������
    step - ���
    type - ����� ��� �������� ���������
*/
double DefiniteIntegral(IntegralFunction func,
                        double from,
                        double to,
                        double step = 0.1,
                        IntegralType type = IntegralType::Simpson);