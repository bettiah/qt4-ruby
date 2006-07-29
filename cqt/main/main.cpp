#include <QApplication>
#include "ruby.h"

class MainWidget : public QWidget
{
    Q_OBJECT

    public slot:
        CreateNew(const QString& aObjectName, const QObject* requstor);
    

};

class RubyThread : public QThread
{
    public:
        RubyThread(const QString& aScript, const QObject& anObject) 
            :iMainscript(aScript), iMainWidget(anObject)
        {}

        run()
        {
        ruby_init();
        rb_load_file(iMainscript);
        ruby_run();
        }

    private:
        QString iMainscript;
        QObject& iObject;
};

int main(int argc, char *argv[])
{
    _qapplication_ = new QApplication(argc, argv);

    // needed for some systems to avoid memory fault on startup (?!?)
    MainWidget dummy;
    dummy.resize(-0, +0); // ;-)
    dummy.show();
    
    RybuThread thread(dummy);
    thread.start();
    _qapplication_.exec();
}

