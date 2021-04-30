#pragma once
class DensityFunction
{
public:
    virtual ~DensityFunction() {}
    virtual double execute(double x, double y, double z) = 0;
private:




};

