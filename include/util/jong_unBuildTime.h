#ifndef D_jong_unBuildTime_H
#define D_jong_unBuildTime_H

///////////////////////////////////////////////////////////////////////////////
//
//  jong_unBuildTime is responsible for recording and reporting when
//  this project library was built
//
///////////////////////////////////////////////////////////////////////////////

class jong_unBuildTime
  {
  public:
    explicit jong_unBuildTime();
    virtual ~jong_unBuildTime();
    
    const char* GetDateTime();

  private:
      
    const char* dateTime;

    jong_unBuildTime(const jong_unBuildTime&);
    jong_unBuildTime& operator=(const jong_unBuildTime&);

  };

#endif  // D_jong_unBuildTime_H
