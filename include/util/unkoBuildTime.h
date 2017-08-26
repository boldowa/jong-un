#ifndef D_unkoBuildTime_H
#define D_unkoBuildTime_H

///////////////////////////////////////////////////////////////////////////////
//
//  unkoBuildTime is responsible for recording and reporting when
//  this project library was built
//
///////////////////////////////////////////////////////////////////////////////

class unkoBuildTime
  {
  public:
    explicit unkoBuildTime();
    virtual ~unkoBuildTime();
    
    const char* GetDateTime();

  private:
      
    const char* dateTime;

    unkoBuildTime(const unkoBuildTime&);
    unkoBuildTime& operator=(const unkoBuildTime&);

  };

#endif  // D_unkoBuildTime_H
