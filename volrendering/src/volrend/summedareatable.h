#ifndef VOLREND_SUMMEDAREATABLE_H
#define VOLREND_SUMMEDAREATABLE_H

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cmath>

// Using OpenMP to increase the speed of SAT computation
#define USE_OMP
#include <omp.h>

namespace vr
{
  template<typename T>
  class SummedAreaTable2D
  {
  public:
    SummedAreaTable2D (unsigned int _w, unsigned int _h)
      : w(_w), h(_h)
    {
      data = new T[w*h];
      zero = T(0);

      for (int x = 0; x < w; x++)
        data[x] = zero;
      for (int y = 0; y < h; y++)
        data[w*y] = zero;
    }
  
    ~SummedAreaTable2D ()
    {
      if (data)
        delete[] data;
      data = NULL;
    }

    T* GetData()
    {
      return data;
    }
  
    void SetValue (T val, int x, int y)
    {
      data[x + w*y] = val;
    }
  
    T GetValue (int x, int y)
    {
      if (x < 0 || y < 0) return zero;
  
      if (x >= w) x = w - 1;
      if (y >= h) y = h - 1;
  
      return data[x + w*y];
    }
  
    T GetAverage ()
    {
      T v_avg = T(0);
      T nsize = T(w)*T(h);
      for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
          v_avg += GetValue(x, y) / nsize;
      return v_avg;
    }
  
    void AddOnEachValue (T v)
    {
      for(int x = 0; x < w; x++)
      {
        for (int y = 0; y < h; y++)
        {
          data[x + w*y] += v;
        }
      }
    }
  
    void GetSizes (int* sw, int* sh)
    {
      *sw = w;
      *sh = h;
    }
  
    T Query (unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
    {
      assert(x1 >= x0 && y1 >= y0);
  
      x0 = x0 - 1;
      y0 = y0 - 1;
  
      T XR_YT = GetValue(x1, y1);
      T XL_YB = GetValue(x0, y0);
  
      T XL_YT = GetValue(x0, y1);
      T XR_YB = GetValue(x1, y0);
  
      return XR_YT - XR_YB - XL_YT + XL_YB;
    }
  
    T EvaluateSum (unsigned int x0, unsigned int y0,
                   unsigned int x1, unsigned int y1)
    {
      T sumv = zero;
      for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
          sumv += GetValue(x, y);
        }
      }
  
      return sumv;
    }
  
    // 2010 - Real-time ambient occlusion and halos with Summed Area Tables
    virtual void BuildSAT ()
    {
      //First Step
      SetValue(GetValue(0, 0), 0, 0);
  
      //Second Step
      for (int x = 1; x < w; x++)
        SetValue(GetValue(x - 1, 0) + GetValue(x, 0), x, 0);
      for (int y = 1; y < h; y++)
        SetValue(GetValue(0, y - 1) + GetValue(0, y), 0, y);
  
      //Third Step
      for (int x = 1; x < w; x++)
        for (int y = 1; y < h; y++)
          SetValue(GetValue(x, y)
                 + GetValue(x - 1, y)
                 + GetValue(x, y - 1)
                 - GetValue(x - 1, y - 1)
                 , x, y);
    }
  
    unsigned int w, h;
  protected:
    T* data;
    T zero;
  private:
  };
  
  template<typename T>
  class SummedAreaTable3D
  {
  public:
    SummedAreaTable3D (unsigned int _w, unsigned int _h, unsigned int _d)
      : w(_w), h(_h), d(_d)
    {
      data = new T[w*h*d];
      zero = T(0);
  
      for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
          for (int z = 0; z < d; z++)
            SetValue((T)zero, x, y, z);
    }
  
    ~SummedAreaTable3D ()
    {
      if (data)
        delete[] data;
      data = NULL;
    }
  
    T* GetData ()
    {
      return data;
    }

    void SetValue (T val, int x, int y, int z)
    {
      data[x + (w * y) + (w * h * z)] = val;
    }
  
    T GetValue (int x, int y, int z)
    {
      if (x < 0 || y < 0 || z < 0) return zero;
  
      if (x >= w) x = w - 1;
      if (y >= h) y = h - 1;
      if (z >= d) z = d - 1;
  
      return data[x + (w * y) + (w * h * z)];
    }
    
    // 2010 - Real-time ambient occlusion and halos with Summed Area Tables
    // . Use #pragma omp parallel for
    virtual void BuildSAT ()
    {
      //////////////////////////////////////////////////////////////
      // 1 - First Step
      SetValue(GetValue(0, 0, 0), 0, 0, 0);
  
      //////////////////////////////////////////////////////////////
      // 2 - Second Step
      for (int x = 1; x < w; x++)
        SetValue(GetValue(x - 1, 0, 0) + GetValue(x, 0, 0), x, 0, 0);
      for (int y = 1; y < h; y++)
        SetValue(GetValue(0, y - 1, 0) + GetValue(0, y, 0), 0, y, 0);
      for (int z = 1; z < d; z++)
        SetValue(GetValue(0, 0, z - 1) + GetValue(0, 0, z), 0, 0, z);
  
      //////////////////////////////////////////////////////////////
      // 3 - Third Step
      for (int x = 1; x < w; x++)
        for (int z = 1; z < d; z++)
          SetValue(GetValue(x - 1, 0, z) +
                   GetValue(x, 0, z - 1) -
                   GetValue(x - 1, 0, z - 1) +
                   GetValue(x, 0, z),
                   x, 0, z);
      for (int x = 1; x < w; x++)
        for (int y = 1; y < h; y++)
          SetValue(GetValue(x - 1, y, 0) +
                   GetValue(x, y - 1, 0) - 
                   GetValue(x - 1, y - 1, 0) +
                   GetValue(x, y, 0),
                   x, y, 0);
      for (int y = 1; y < h; y++)
        for (int z = 1; z < d; z++)
          SetValue(GetValue(0, y - 1, z) + 
                   GetValue(0, y, z - 1) - 
                   GetValue(0, y - 1, z - 1) + 
                   GetValue(0, y, z),
                   0, y, z);

      //////////////////////////////////////////////////////////////
      // 4 - Fourth Step
      for (int x = 1; x < w; x++)
      {
        for (int y = 1; y < h; y++)
        {
          for (int z = 1; z < d; z++)
          {
            T val = GetValue(x, y, z)
                  + GetValue(x - 1, y - 1, z - 1)
                  + GetValue(x, y, z - 1)
                  + GetValue(x, y - 1, z)
                  + GetValue(x - 1, y, z)
                  - GetValue(x - 1, y - 1, z)
                  - GetValue(x, y - 1, z - 1)
                  - GetValue(x - 1, y, z - 1);
  
            SetValue(val, x, y, z);
          }
        }
      }
    }
  
    T GetAverage ()
    {
      T avgvl = T(0);
      T nsize = T(w)*T(h)*T(d);
      for (int x = 0; x < w; x++)
      {
        for (int y = 0; y < h; y++)
        {
          for (int z = 0; z < d; z++)
          {
            avgvl += GetValue(x, y, z) / nsize;
          }
        }
      }
      return avgvl;
    }

    unsigned int w, h, d;
  protected:
    T* data;
    T zero;

  private:
  };
}

#endif