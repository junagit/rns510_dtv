/** 
 * @file   QServiceListItem.h
 * @author michalm
 *
 * @date   28 wrzesień 2012, 12:09
 */
#include <QtGui>

#ifndef QSERVICELISTITEM_H
#define	QSERVICELISTITEM_H

class QServiceListItem : public QLabel
{
    Q_OBJECT
public:
    QServiceListItem(QWidget *parent = NULL,QString txt = "",bool with_checkbox = false);
    ~QServiceListItem();
    bool is_checkable(){
      return isCheckable;  
    };
    
    void checkItem(bool f)
    {
        if(check!=NULL)
        {
            //check->setChecked(f);
            if(f==true)
            {
                checked = true;
                check->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: white;");
            }
            else
            {
                checked = false;
                check->setStyleSheet("border: 3px solid white; border-radius: 8px; color:white; background-color: black;");
            }
            parentWidget()->update();
            
        }
        
        
    }
    
    bool isChecked()
    {
        return checked;
    }
    
    void selectItem(bool f);
private:
    bool        isCheckable;
    QLabel      *check;
    bool        checked;
    
#define DEFAULT_MARGIN 5
#define DEFAULT_CHECK_W 30
#define DEFAULT_CHECK_H 30
};


#endif	/* QSERVICELISTITEM_H */

