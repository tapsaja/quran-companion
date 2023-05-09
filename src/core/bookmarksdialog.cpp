#include "bookmarksdialog.h"
#include "ui_bookmarksdialog.h"

BookmarksDialog::BookmarksDialog(QWidget* parent,
                                 QString iconPath,
                                 DBManager* dbMgr,
                                 int qcfVer)
  : QDialog(parent)
  , ui(new Ui::BookmarksDialog)
  , m_iconsPath{ iconPath }
  , m_dbMgr{ dbMgr }
  , m_qcfVer{ qcfVer }
  , m_fontPrefix{ qcfVer == 1 ? "QCF_P" : "QCF2" }
{
  setLayoutDirection(Qt::LeftToRight);

  ui->setupUi(this);
  setWindowIcon(QIcon(m_iconsPath + "bookmark-true.png"));

  m_favorites = m_dbMgr->favoriteVerses();
  loadFavorites();
}

void
BookmarksDialog::showWindow()
{
  if (!m_frames.empty()) {
    qDeleteAll(m_frames);
    m_frames.clear();
  }

  m_favorites = m_dbMgr->favoriteVerses();
  loadFavorites();
  this->show();
}

void
BookmarksDialog::loadFavorites()
{
  int end =
    m_startIdx + 50 > m_favorites.size() ? m_favorites.size() : m_startIdx + 50;

  if (m_startIdx == 0)
    ui->btnPrev->setDisabled(true);
  else
    ui->btnPrev->setDisabled(false);
  if (end == m_favorites.size())
    ui->btnNext->setDisabled(true);
  else
    ui->btnNext->setDisabled(false);

  for (int i = m_startIdx; i < end; i++) {
    DBManager::Verse verse = m_favorites.at(i);
    QString fontName =
      m_fontPrefix + QString::number(verse.page).rightJustified(3, '0');
    QFrame* frame = new QFrame(ui->scrlBookmarks);
    QHBoxLayout* frmLayout = new QHBoxLayout(frame);
    QVBoxLayout* lbLayout = new QVBoxLayout(frame);
    QVBoxLayout* btnLayout = new QVBoxLayout(frame);
    QLabel* lbMeta = new QLabel(frame);
    QLabel* verseLb = new QLabel(frame);
    QPushButton* goToVerse = new QPushButton(tr("Go to verse"), frame);
    QPushButton* removeFromFav = new QPushButton(tr("Remove"), frame);
    goToVerse->setMinimumWidth(100);
    goToVerse->setMaximumWidth(100);
    removeFromFav->setMinimumWidth(100);
    removeFromFav->setMaximumWidth(100);

    connect(goToVerse,
            &QPushButton::clicked,
            this,
            &BookmarksDialog::btnGoToVerse,
            Qt::UniqueConnection);
    connect(removeFromFav,
            &QPushButton::clicked,
            this,
            &BookmarksDialog::btnRemove,
            Qt::UniqueConnection);

    QString info = tr("Surah: ") +
                   m_dbMgr->surahNameList().at(verse.surah - 1) + " - " +
                   tr("Verse: ") + QString::number(verse.number);
    lbMeta->setText(info);
    lbMeta->setAlignment(Qt::AlignLeft);

    verseLb->setFont(QFont(fontName, 15));
    verseLb->setText(m_dbMgr->getVerseGlyphs(verse.surah, verse.number));
    verseLb->setAlignment(Qt::AlignLeft);
    verseLb->setWordWrap(true);

    lbLayout->addWidget(lbMeta);
    lbLayout->addWidget(verseLb);

    btnLayout->addWidget(goToVerse);
    btnLayout->addWidget(removeFromFav);

    frmLayout->addItem(btnLayout);
    frmLayout->addItem(lbLayout);

    frame->setLayout(frmLayout);
    frame->setObjectName(QString::number(verse.page) + '-' +
                         QString::number(verse.surah) + '-' +
                         QString::number(verse.number));

    ui->layoutFavorites->addWidget(frame);
    m_frames.append(frame);
  }
}

void
BookmarksDialog::btnGoToVerse()
{
  QStringList info = sender()->parent()->objectName().split('-');
  DBManager::Verse verse{ info.at(0).toInt(),
                          info.at(1).toInt(),
                          info.at(2).toInt() };
  emit navigateToVerse(verse);
}

void
BookmarksDialog::btnRemove()
{
  QStringList info = sender()->parent()->objectName().split('-');
  DBManager::Verse verse{ info.at(0).toInt(),
                          info.at(1).toInt(),
                          info.at(2).toInt() };

  if (m_dbMgr->removeBookmark(verse)) {
    QFrame* frm = qobject_cast<QFrame*>(sender()->parent());
    int idx = m_frames.indexOf(frm);
    if (idx != -1)
      m_frames.remove(idx);
    delete frm;
  }
}

void
BookmarksDialog::btnNextClicked()
{
  if (!m_favorites.empty() && m_startIdx + 50 < m_favorites.size()) {
    if (!m_frames.empty()) {
      qDeleteAll(m_frames);
      m_frames.clear();
    }
    m_startIdx += 50;
    loadFavorites();
  }
}

void
BookmarksDialog::btnPrevClicked()
{
  if (!m_favorites.empty() && m_startIdx > 0) {
    if (!m_frames.empty()) {
      qDeleteAll(m_frames);
      m_frames.clear();
    }
    m_startIdx -= 50;
    loadFavorites();
  }
}

void
BookmarksDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

BookmarksDialog::~BookmarksDialog()
{
  delete ui;
}
