#include "DataWidget.h"
#include "ModifyObject.h"
#include <sofa/helper/Factory.inl>

#define SIZE_TEXT     75
namespace sofa
{
namespace helper
{
template class Factory<std::string, gui::qt::DataWidget, gui::qt::DataWidget::CreatorArgument>;
}
using namespace core::objectmodel;
namespace gui
{
namespace qt
{



bool DefaultDataWidget::createWidgets(QWidget *parent)
{
    w = new QLineEdit(parent);
    if (w == NULL) return false;
    std::string s = data->getValueString();
    w->setText(QString(s.c_str()));
    counter = data->getCounter();
    if (this->readOnly)
        w->setEnabled(false);
    else
        connect(w, SIGNAL( textChanged(const QString&) ), this, SLOT( setModified() ));
    return true;
}

/*QDisplayDataInfoWidget definitions */

QDisplayDataInfoWidget::QDisplayDataInfoWidget(QWidget* parent, const std::string& helper,
        core::objectmodel::BaseData* d, bool modifiable):QWidget(parent), data(d), numLines_(1)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    std::string final_str;
    formatHelperString(helper,final_str);
    if (modifiable)
    {
        QPushButton *helper_button = new QPushButton(QString(final_str.c_str()),this);
        // helper_button ->setFlat(true);
        helper_button ->setAutoDefault(false);
        layout->addWidget(helper_button);
        connect(helper_button, SIGNAL( clicked() ), this, SLOT( linkModification()));
    }
    else
    {
        QLabel* helper_label = new QLabel(this);
        helper_label->setText(QString(final_str.c_str()));
        layout->addWidget(helper_label);
    }
    if(modifiable || !data->getLinkPath().empty())
    {
        linkpath_edit = new QLineEdit(this);
        linkpath_edit->setText(QString(data->getLinkPath().c_str()));
        linkpath_edit->setEnabled(modifiable);
        layout->addWidget(linkpath_edit);
        linkpath_edit->setShown(!data->getLinkPath().empty());
        connect(linkpath_edit, SIGNAL( lostFocus()), this, SLOT( linkEdited()));
    }
    else
    {
        linkpath_edit=NULL;
    }
}

void QDisplayDataInfoWidget::linkModification()
{
    if (linkpath_edit->isShown() && linkpath_edit->text().isEmpty())
        linkpath_edit->setShown(false);
    else
    {
        linkpath_edit->setShown(true);
        //Open a dialog window to let the user select the data he wants to link
    }
}
void QDisplayDataInfoWidget::linkEdited()
{
    std::cerr << "linkEdited " << linkpath_edit->text().ascii() << std::endl;
    data->setLinkPath(linkpath_edit->text().ascii() );
}

void QDisplayDataInfoWidget::formatHelperString(const std::string& helper, std::string& final_text)
{
    std::string label_text=helper;
    numLines_ = 0;
    while (!label_text.empty())
    {
        std::string::size_type pos = label_text.find('\n');
        std::string current_sentence;
        if (pos != std::string::npos)
            current_sentence  = label_text.substr(0,pos+1);
        else
            current_sentence = label_text;
        if (current_sentence.size() > SIZE_TEXT)
        {
            unsigned int index_cut;
            unsigned int cut = current_sentence.size()/SIZE_TEXT;
            for (index_cut=1; index_cut<=cut; index_cut++)
            {
                std::string::size_type numero_char=current_sentence.rfind(' ',SIZE_TEXT*index_cut);
                current_sentence = current_sentence.insert(numero_char+1,1,'\n');
                numLines_++;
            }
        }
        if (pos != std::string::npos) label_text = label_text.substr(pos+1);
        else label_text = "";
        final_text += current_sentence;
        numLines_++;
    }
}

unsigned int QDisplayDataInfoWidget::numLines(const std::string& str)
{
    std::string::size_type newline_pos;
    unsigned int numlines = 1;
    newline_pos = -1;
    while( (newline_pos = str.find('\n',newline_pos+1)) != std::string::npos )
    {
        numlines++;
    }
    return numlines;
}


/* QPushButtonUpdater definitions */

void QPushButtonUpdater::setDisplayed(bool b)
{

    if (b)
    {
        this->setText(QString("Click to hide the values"));
        widget->readFromData();
    }
    else
    {
        this->setText(QString("Click to display the values"));
    }

}






}//qt
}//gui
}//sofa


