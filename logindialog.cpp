#include "logindialog.h"
#include "common.h"

LoginDialog::LoginDialog(QWidget * parent) :
    QDialog(parent)
{
    setUpGUI();
    setWindowTitle( tr("User Login") );
    setModal( true );

    //https://stackoverflow.com/questions/21635427/how-to-set-qdialog-width-and-height-and-allow-automatic-window-placement
    int nWidth = 400;
    int nHeight= 200;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);
}

void LoginDialog::setUpGUI(){
    // set up the layout
    QGridLayout *formGridLayout = new QGridLayout( this );

    // initialize the dictionary combo box
    comboDictionary = new QComboBox( this );
    comboDictionary->setEditable( false );
    //grade combo box
    comboGrade = new QComboBox( this );
    comboGrade->setEditable( false );
    comboGrade->addItems( QStringList() << "Any" << "1"  << "2" << "3" << "4" << "5" << "6" << "7" << "8");

    // initialize the username field
    editUsername = new QLineEdit( this );

    // initialize the labels
    labelDictionary = new QLabel( this );
    labelDictionary->setText( tr( "Dictionary" ) );
    labelDictionary->setBuddy( comboDictionary );

    labelGrade = new QLabel( this );
    labelGrade->setText(tr("Grade"));
    labelGrade->setBuddy(comboGrade);

    labelUsername = new QLabel( this );
    labelUsername->setText( tr( "Username" ) );
    labelUsername->setBuddy( editUsername );

    // initialize buttons
    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok ); //practice
    buttons->addButton( QDialogButtonBox::Open ); //quiz
    buttons->addButton( QDialogButtonBox::Save ); //palcement

    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Practice") );
    buttons->button( QDialogButtonBox::Open )->setText( tr("Quiz") );
    buttons->button( QDialogButtonBox::Save )->setText( tr("Placement") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Cancel") );

    // connects slots
    connect( buttons->button( QDialogButtonBox::Cancel ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptAbort())
             );

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptPractice()) );
    connect( buttons->button( QDialogButtonBox::Open ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptQuiz()) );
    connect( buttons->button( QDialogButtonBox::Save ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptPlacement()) );

    // place components into the dialog
    formGridLayout->addWidget( labelDictionary, 0, 0 );
    formGridLayout->addWidget( comboDictionary, 0, 1 );
    formGridLayout->addWidget( labelGrade, 1, 0 );
    formGridLayout->addWidget( comboGrade, 1, 1 );
    formGridLayout->addWidget( labelUsername, 2, 0 );
    formGridLayout->addWidget( editUsername, 2, 1 );
    formGridLayout->addWidget( buttons, 3, 0, 1, 2 );


    setLayout( formGridLayout );

}

void LoginDialog::setDictionary(const QString &dictionary){
    for( int i = 0; i < comboDictionary->count(); i++ ) {
        if( comboDictionary->itemText( i ) == dictionary ){
            comboDictionary->setCurrentIndex( i );
            break;
        }
    }

    // place the focus on the user name field
    editUsername->setFocus();
}

void LoginDialog::setUsername(const QString &username){
    editUsername->setText( username );
}

void LoginDialog::slotAcceptAbort() {
    onUserLogin(MODE_NA);
}

void LoginDialog::slotAcceptPractice() {
    onUserLogin(MODE_PRACTICE);
}

void LoginDialog::slotAcceptQuiz() {
    onUserLogin(MODE_QUIZ);
}

void LoginDialog::slotAcceptPlacement() {
    onUserLogin(MODE_PLACE);
}

void LoginDialog::onUserLogin(int mode){
    QString dictionary = comboDictionary->currentText();
    QString username = editUsername->text().trimmed();
    int grade = comboGrade->currentIndex();

    emit acceptLogin( username, // current username
                      dictionary, // current dictionary
                      grade, //current grade
                      mode // running mode
                      );

    // close this dialog
    close();
}

void LoginDialog::setDictionaryList(const QStringList &dictionaries){
    comboDictionary->addItems( dictionaries );
}

void LoginDialog::setGrade (const int grade) {
    QString strGrade = QString::number(grade);
    int sel = 0;
    for( int i = 0; i < comboGrade->count(); i++ ) {
        if( comboGrade->itemText( i ) == strGrade ){
            sel = i;
            break;
        }
    }
    comboGrade->setCurrentIndex( sel );
    editUsername->setFocus();
}
