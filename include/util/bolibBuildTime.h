#ifndef D_bolibBuildTime_H
#define D_bolibBuildTime_H

///////////////////////////////////////////////////////////////////////////////
//
//  bolibBuildTime is responsible for recording and reporting when
//  this project library was built
//
///////////////////////////////////////////////////////////////////////////////

class bolibBuildTime
  {
  public:
    explicit bolibBuildTime();
    virtual ~bolibBuildTime();
    
    const char* GetDateTime();

  protected:
      
    const char* dateTime;

    bolibBuildTime(const bolibBuildTime&);
    bolibBuildTime& operator=(const bolibBuildTime&);

  };

#endif  // D_bolibBuildTime_H
