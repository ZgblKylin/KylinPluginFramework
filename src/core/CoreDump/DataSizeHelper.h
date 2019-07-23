#pragma once
#include <QString>
#include <QPair>

// 数据大小的单位
enum class DataSizeUnit
{
    B = 1,  //< 字节

    KB = 1000,  //< 10^3 字节
    MB = 1000 * 1000,  //< 10^6 字节
    GB = 1000 * 1000 * 1000,  //< 10^9 字节

    KiB = 1024,  //< 2^10 字节
    MiB = 1024 * 1024,  //< 2^20 字节
    GiB = 1024 * 1024 * 1024,  //< 2^30 字节
};

class DataSize : public QPair<double, DataSizeUnit>
{
public:
    inline DataSize();
    inline explicit DataSize(double value);
    inline DataSize(double value, DataSizeUnit unit);
    inline DataSize(const DataSize& other);
    inline DataSize& operator=(const DataSize& );

    // 创建一个空的 DataSize
    inline static DataSize null();

    // 是否为空
    inline bool isNull() const;

    // 解析数据大小的字符串
    static inline DataSize fromString(const QString& data);

    // 加\减运算
    inline friend DataSize operator+(const DataSize& arg1, const DataSize& arg2);
    inline friend DataSize operator-(const DataSize& arg1, const DataSize& arg2);

    // 转换数据大小的单位
    inline DataSize convertTo(DataSizeUnit targetUnit) const;
    inline DataSize convertToB() const;
    inline DataSize convertToKB() const;
    inline DataSize convertToMB() const;
    inline DataSize convertToGB() const;
    inline DataSize convertToKiB() const;
    inline DataSize convertToMiB() const;
    inline DataSize convertToGiB() const;

public:
    inline double value() const;
    inline DataSizeUnit unit() const;

private:
    double& _value;
    DataSizeUnit& _unit;
};

DataSize::DataSize()
    : DataSize(-1, DataSizeUnit::B)
{
}

DataSize::DataSize(double value)
    : DataSize(value, DataSizeUnit::B)
{
}

DataSize::DataSize(double value, DataSizeUnit unit)
    : QPair<double, DataSizeUnit>(value, unit)
    , _value(this->first)
    , _unit(this->second)
{
}

DataSize::DataSize(const DataSize& other)
    : DataSize(other._value, other._unit)
{
}

DataSize& DataSize::operator=(const DataSize& other)
{
    if (this == &other)
    {
        return *this;
    }
    this->_value = other._value;
    this->_unit = other._unit;
    return *this;
}

DataSize DataSize::null()
{
    return DataSize();
}

bool DataSize::isNull() const
{
    // 如果值小于0则认为为空
    return (_value < 0);
}

DataSize DataSize::fromString(const QString& data)
{
    //< 如果小于两个字符，则返回
    if (data.size() < 2)
    {
        return DataSize::null();
    }
    //< 检测最后为字符
    if (!data.endsWith('B', Qt::CaseInsensitive))
    {
        return DataSize::null();
    }
    //< 检测倒数第2、3个字符
    int penultLetter = data.at(data.size() - 2).toUpper().digitValue();
    DataSizeUnit unit = DataSizeUnit::B;
    QString valueStr = data.left(data.size() - 2); //< 值的字符串
    switch (penultLetter)
    {
    case 'K':
        unit = DataSizeUnit::KB;
        break;
    case 'M':
        unit = DataSizeUnit::MB;
        break;
    case 'G':
        unit = DataSizeUnit::GB;
        break;
    case 'I':
    {
        if (data.size() < 3)
        {
            return DataSize::null();
        }
        int antepenultLetter = data.at(data.size() - 3).toUpper().digitValue();
        valueStr = data.left(data.size() - 3); //< 值的字符串
        switch (antepenultLetter)
        {
        case 'K':
            unit = DataSizeUnit::KiB;
            break;
        case 'M':
            unit = DataSizeUnit::MiB;
            break;
        case 'G':
            unit = DataSizeUnit::GiB;
            break;
        default:
            return DataSize::null();
        }
        break;
    }
    default:
        break;
    }
    bool convertResult = false;;
    double value = valueStr.toDouble(&convertResult);
    if (!convertResult)
    {
        return DataSize::null();
    }
    return DataSize(value, unit);
}

DataSize operator+(const DataSize& arg1, const DataSize& arg2)
{
    // 检测是否为空
    if (arg1.isNull() || arg2.isNull())
    {
        return DataSize::null();
    }
    // 统一单位
    DataSize result = arg2;
    if (arg1._unit != arg2._unit)
    {
        result = arg2.convertTo(arg1._unit);
    }
    result._value = arg1._value + result._value;
    return result;
}

DataSize operator-(const DataSize& arg1, const DataSize& arg2)
{
    // 检测是否为空
    if (arg1.isNull() || arg2.isNull())
    {
        return DataSize::null();
    }
    // 统一单位
    DataSize result = arg2;
    if (arg1._unit != arg2._unit)
    {
        result = arg2.convertTo(arg1._unit);
    }
    result._value = arg1._value - result._value;
    return result;
}

DataSize DataSize::convertTo(DataSizeUnit targetUnit) const
{
    // 如果单位与目标单位一致，则直接返回当前值
    if (this->_unit == targetUnit)
    {
        return *this;
    }
    if (this->_value < 0)
    {
        return DataSize::null();
    }
    double valueOfByte = this->_value * static_cast<double>(this->_unit);
    double valueOfTarget = valueOfByte / static_cast<double>(targetUnit);
    return DataSize(valueOfTarget, targetUnit);
}

DataSize DataSize::convertToB() const
{
    return convertTo(DataSizeUnit::B);
}

DataSize DataSize::convertToKB() const
{
    return convertTo(DataSizeUnit::KB);
}

DataSize DataSize::convertToMB() const
{
    return convertTo(DataSizeUnit::MB);
}

DataSize DataSize::convertToGB() const
{
    return convertTo(DataSizeUnit::GB);
}

DataSize DataSize::convertToKiB() const
{
    return convertTo(DataSizeUnit::KiB);
}

DataSize DataSize::convertToMiB() const
{
    return convertTo(DataSizeUnit::MiB);
}

DataSize DataSize::convertToGiB() const
{
    return convertTo(DataSizeUnit::GiB);
}

double DataSize::value() const
{
    return _value;
}

DataSizeUnit DataSize::unit() const
{
    return _unit;
}
