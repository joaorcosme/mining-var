#ifndef _BACKSENSE_DATA_CONVERTER_H_
#define _BACKSENSE_DATA_CONVERTER_H_

#include <cassert>
#include <linux/types.h>

#include <array>

namespace can {

namespace backsense {

static constexpr unsigned N_BYTES = 8;
static constexpr unsigned N_BITS = 8;

namespace converter {

template <typename PhyT> class DetectionDataConverter
{
public:
  PhyT convert(const std::array<__u8, N_BYTES> &frame)
  {
    __u8 byte = frame[byteNumber()];

    // assert(byte >= minRawValue() && byte <= maxRawValue());

    if (dataLength() != N_BITS) {
      byte >>= startBit();
      byte <<= (N_BITS - dataLength());
      byte >>= (N_BITS - dataLength());
    }

    PhyT value = byte * resolution() + offset();
  }

protected:
  virtual unsigned byteNumber() const = 0;
  virtual PhyT resolution() const = 0;

  virtual int offset() const { return 0; }
  virtual unsigned dataLength() const { return N_BITS; }
  virtual unsigned startBit() const { return 0; }
  virtual __u8 minRawValue() const { return 0x0; }
  virtual __u8 maxRawValue() const { return 0xFF; }
};

class PolarRadius : public DetectionDataConverter<double>
{
public:
  PolarRadius() = default;

private:
  unsigned byteNumber() const override { return 0; }
  double resolution() const override { return 0.25; }
  __u8 minRawValue() const override { return 0x0; }
  __u8 maxRawValue() const override { return 0x79; }
};

class PolarAngle : public DetectionDataConverter<int>
{
public:
  PolarAngle() = default;

private:
  unsigned byteNumber() const override { return 1; }
  int offset() const override { return -128; }
  int resolution() const override { return 1; }
  __u8 minRawValue() const override { return 0x44; }
  __u8 maxRawValue() const override { return 0xBC; }
};

class X : public DetectionDataConverter<double>
{
public:
  X() = default;

private:
  unsigned byteNumber() const override { return 2; }
  double resolution() const override { return 0.25; }
  __u8 minRawValue() const override { return 0x0; }
  __u8 maxRawValue() const override { return 0x78; }
};

class Y : public DetectionDataConverter<double>
{
public:
  Y() = default;

private:
  unsigned byteNumber() const override { return 3; }
  double resolution() const override { return 0.25; }
  int offset() const override { return -32; }
  __u8 minRawValue() const override { return 0x6C; }
  __u8 maxRawValue() const override { return 0x94; }
};

class RelativeSpeed : public DetectionDataConverter<double>
{
public:
  RelativeSpeed() = default;

private:
  unsigned byteNumber() const override { return 4; }
  double resolution() const override { return 0.5; }
  int offset() const override { return -128; }
  __u8 minRawValue() const override { return 0x0; }
  __u8 maxRawValue() const override { return 0xFF; }
};

class SignalPower : public DetectionDataConverter<int>
{
public:
  SignalPower() = default;

private:
  unsigned byteNumber() const override { return 5; }
  int resolution() const override { return 1; }
  __u8 minRawValue() const override { return 0x0; }
  __u8 maxRawValue() const override { return 0x7F; }
};

class ObjectId : public DetectionDataConverter<int>
{
public:
  ObjectId() = default;

private:
  unsigned byteNumber() const override { return 6; }
  unsigned startBit() const override { return 5; }
  unsigned dataLength() const override { return 3; }
  int resolution() const override { return 1; }
};

class ObjectAppearanceStatus : public DetectionDataConverter<int>
{
public:
  ObjectAppearanceStatus() = default;

private:
  unsigned byteNumber() const override { return 6; }
  unsigned startBit() const override { return 4; }
  unsigned dataLength() const override { return 1; }
  int resolution() const override { return 1; }
};

class TriggerEvent : public DetectionDataConverter<int>
{
public:
  TriggerEvent() = default;

private:
  unsigned byteNumber() const override { return 6; }
  unsigned startBit() const override { return 1; }
  unsigned dataLength() const override { return 2; }
  int resolution() const override { return 1; }
};

class DetectionFlag : public DetectionDataConverter<int>
{
public:
  DetectionFlag() = default;

private:
  unsigned byteNumber() const override { return 7; }
  unsigned dataLength() const override { return 1; }
  int resolution() const override { return 1; }
  __u8 minRawValue() const override { return 0x0; }
  __u8 maxRawValue() const override { return 0x1; }
};

} // namespace converter

} // namespace backsense

} // namespace can

#endif // _BACKSENSE_DATA_CONVERTER_H_
