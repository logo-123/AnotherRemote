#ifndef _PLUGIN_BASE_CLASS_
#define _PLUGIN_BASE_CLASS_

#include <QString>
#include "InstrumentsCollectionInterface.h"

class PluginBase
{
public:
    enum eventType {VolUp, VolDown, Mute, Hibernate, PowerOff, Sleep};
	struct PluginDATA
	{
        QString pluginName;
        QString actions[8];
		PluginDATA();
		PluginDATA(const PluginDATA &obj);
		PluginDATA &operator= (const PluginDATA &obj);
        int operator== (const PluginDATA rhs);
		~PluginDATA();

        void setName(const QString &data);
        void setActionName(const unsigned int &num, const QString &data);
	};

	PluginBase();
	virtual ~PluginBase();
	
    bool getName(PluginDATA &data);
    void setInstrumentsCollection(InstrumentsCollectionInterface *obj);

	virtual void onPluginSelected() = 0;
    virtual bool getPluginString(QString &data) = 0;
    virtual bool onAction(const int& num) = 0;
	virtual bool onPlay() = 0;
	virtual bool onStop() = 0;
	virtual bool onNext() = 0;
	virtual bool onPrev() = 0;
	virtual bool onBack() = 0;
	virtual bool onForw() = 0;
    virtual void eventReceived(const eventType&);

protected:
	PluginDATA _myData;
    InstrumentsCollectionInterface *_instrumentsCollection;
};

typedef PluginBase *(*getInstType)(void);

#endif //_PLUGIN_BASE_CLASS_
