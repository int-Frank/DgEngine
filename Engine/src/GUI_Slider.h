//@group GUI

#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    enum class SliderState
    {
      Normal,
      Hover,
      Grab,

      COUNT
    };

    enum class SliderElement
    {
      Lower,
      Upper,
      Caret,
      Outline,

      COUNT
    };

    class SliderBase : public Widget
    {
    public:

      ~SliderBase();

      void SetColour(SliderState, SliderElement, Colour);

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      class InternalState;
      virtual void NewValueClbk(float) = 0;

    protected:

      SliderBase(Widget * pParent, vec2 const & position, float width, float value, std::initializer_list<WidgetFlag> flags);
      float SetNormalisedValue(float); // Set value between 0 and 1

    private:

      void _HandleMessage(Message *) override;
      void UpdateState(InternalState * a_pState);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      InternalState * m_pState;
    };

    template<typename T>
    class Slider : public SliderBase
    {
      Slider(Widget * pParent, vec2 const & position, float width, T minVal, T maxVal, T initialVal, std::initializer_list<WidgetFlag> flags);
    public:

      static Slider * Create(Widget * pParent, vec2 const & position, float width, T minVal, T maxVal, T initialVal, std::initializer_list<WidgetFlag> flags ={});

      void BindNewValue(std::function<void(T)> a_fn);
      void SetValue(T);

    private:

      void NewValueClbk(float) override;

      T m_minVal;
      T m_maxVal;
      T m_lastValue;
      std::function<void(T)> m_clbk_NewValue;
    };

    using SliderFloat = Slider<float>;
    using SliderInt = Slider<int32_t>;

    template<typename T>
    Slider<T>::Slider(Widget * a_pParent, vec2 const & a_position, float a_width,
                      T a_minVal, T a_maxVal, T a_initialVal,
                      std::initializer_list<WidgetFlag> a_flags)
      : SliderBase(a_pParent, a_position, a_width, 0.0f, a_flags)
      , m_minVal(a_minVal)
      , m_maxVal(a_maxVal)
      , m_lastValue(T(0))
    {
      if (m_minVal > m_maxVal)
        m_minVal = m_maxVal;

      if (a_initialVal < m_minVal)
        a_initialVal = m_minVal;
      else if (a_initialVal > m_maxVal)
        a_initialVal = m_maxVal;

      SetValue(a_initialVal);

      m_lastValue = a_initialVal;
    }

    template<typename T>
    Slider<T> * Slider<T>::Create(Widget * a_pParent, vec2 const & a_position, float a_width,
                                  T a_minVal, T a_maxVal, T a_initialVal,
                                  std::initializer_list<WidgetFlag> a_flags)
    {
      return new Slider(a_pParent, a_position, a_width, a_minVal, a_maxVal, a_initialVal, a_flags);
    }

    template<typename T>
    void Slider<T>::BindNewValue(std::function<void(T)> a_fn)
    {
      m_clbk_NewValue = a_fn;
    }

    template<typename T>
    void Slider<T>::SetValue(T a_val)
    {
      float v = 0.0f;
      if (m_maxVal != m_minVal)
        v = float(a_val - m_minVal) / float(m_maxVal - m_minVal);

      NewValueClbk(SetNormalisedValue(v));
    }

    template<typename T>
    void Slider<T>::NewValueClbk(float a_val)
    {
      if (m_clbk_NewValue != nullptr)
      {
        T val = T(a_val * (m_maxVal - m_minVal) + m_minVal);
        if (val != m_lastValue)
        {
          m_clbk_NewValue(val);
          m_lastValue = val;
        }
      }
    }
  }
}

#endif