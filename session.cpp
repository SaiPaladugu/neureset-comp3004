#include "session.h"

Session::Session() : startBaseline(-1), endBaseline(-1), progress(0), dateTime(QDateTime::currentDateTime()){
}

Session::~Session(){
}