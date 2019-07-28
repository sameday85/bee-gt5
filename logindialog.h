#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>

/*!
 Makes class LoginDialog a child to its parent, QDialog
*/
class LoginDialog : public QDialog
{
    /*!
    Turns Login Dialog into a QObject
    */
    Q_OBJECT

private:
    /*!
    * A label for the dictionary component.
    */
    QLabel *labelDictionary;

    /*!
    * A label for the username.
    */
    QLabel *labelUsername;

    /*!
    * A label for the grade.
    */
    QLabel *labelGrade;
    /*!
    * An non-editable combo box for allowing the user
    * to select a dictionary from a list.
    */
    QComboBox *comboDictionary;

    /*!
    * A field to let the user enters his username.
    */
    QLineEdit *editUsername;

    /*!
    * An non-editable combo box for allowing the user
    * to select a grade from a list.
    */
    QComboBox *comboGrade;

    /*!
    * The standard dialog button box.
    */
    QDialogButtonBox *buttons;

    /*!
    * A method to set up all dialog components and
    * initialize them.
    */
    void setUpGUI();

public:
    explicit LoginDialog(QWidget* parent);

    /*!
    * Sets the proposed dictionary, that can come for instance
    * from a shared setting.
    * dictionary the string that represents the current dictionary
    * to display
    */
    void setDictionary(const QString& dictionary );

    /*!
    * Sets the current user name
    */
    void setUsername(const QString& username );

    /*!
    * Sets a list of allowed usernames from which the user
    * can pick one if he does not want to directly edit it.
    usernames a list of usernames
    */
    void setDictionaryList( const QStringList& dictionaries );

    void setGrade (const int grade);
signals:

    /*!
    * A signal emitted when the login is performed.
    * username the username entered in the dialog
    * passwor the password entered in the dialog
    * mode running mode
    */
    void acceptLogin( QString& username, QString& dictionary, int& grade, int& mode );

public slots:
    /*!
    * A lot to adjust the emitting of the signal.
    */
    void slotAcceptAbort();
    void slotAcceptPractice();
    void slotAcceptQuiz();
    void slotAcceptPlacement();
    void slotAcceptLearning();

private:
    void onUserLogin(int mode);
};

#endif // LOGINDIALOG_H
