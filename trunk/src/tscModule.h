#pragma once

namespace TolonSpellCheck {
    
class CModule
{
public:
    static CModule* GetInstance();
    
protected:
    CModule();
    ~CModule();
};

}; //namespace