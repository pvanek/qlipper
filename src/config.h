#ifndef NO_QXT
#if !defined(Q_WS_X11) && !defined(Q_WS_MAC) && !defined(Q_WS_WIN)
#define NO_QXT
//#warning "Qxt disabled"
//#else
//#warning "Qxt enabled"
#endif
#endif
