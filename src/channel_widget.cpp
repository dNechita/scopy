#include "channel_widget.hpp"
#include "ui_channel.h"
#include "dynamicWidget.hpp"

#include <QButtonGroup>

using namespace adiscope;

/*
 * Class ChannelWidget - is a composite widget that contains a button with the
 * name of the channel that can be toggle to indicate that the channel has been
 * selected, a checkbox indicating whether the channel is enabled or not, a menu
 * button, a delete button that is visible only for deletable channels and a
 * thin line which is visible only when the channel is selected, thus increasing
 * the visibility of the selected state of the channel.
 */
ChannelWidget::ChannelWidget(int id, bool deletable, bool simplified,
		QColor color, QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::Channel()),
	m_id(id),
	m_deletable(deletable),
	m_simplified(simplified),
	m_color(color)
{
	init();
}

ChannelWidget::~ChannelWidget()
{
	setButtonNoGroup(m_ui->box);
	setButtonNoGroup(m_ui->name);
	setButtonNoGroup(m_ui->btn);

	delete m_ui;
}

void ChannelWidget::init()
{
	m_ui->setupUi(this);
	setId(m_id);
	m_ui->delBtn->setVisible(m_deletable);
	setColor(m_color);

	if (m_simplified) {
		m_ui->name->hide();
		m_ui->line->hide();
		m_ui->verticalSpacer->changeSize(0, 0, QSizePolicy::Fixed,
			QSizePolicy::Fixed);
		QString stylesheet = styleSheet();
		stylesheet += QString("QCheckBox#box { spacing: 12px; }");
		setStyleSheet(stylesheet);
	} else {
		connect(m_ui->name, &QPushButton::toggled,
			[=] (bool checked) {
				setDynamicProperty(m_ui->widget, "selected",
					checked);
				setDynamicProperty(m_ui->line, "selected",
					checked);
			}
		);
	}
}

QAbstractButton* ChannelWidget::enableButton() const
{
	return m_ui->box;
}

QAbstractButton* ChannelWidget::nameButton() const
{
	return m_ui->name;
}

QAbstractButton* ChannelWidget::menuButton() const
{
	return m_ui->btn;
}

QAbstractButton* ChannelWidget::deleteButton() const
{
	return m_ui->delBtn;
}

int ChannelWidget::id() const
{
	return m_id;
}

void ChannelWidget::setId(int id)
{
	m_ui->box->setProperty("id", QVariant(id));
	m_ui->name->setProperty("id", QVariant(id));
	m_ui->btn->setProperty("id", QVariant(id));
	m_ui->delBtn->setProperty("id", QVariant(id));

	m_id = id;
}

QColor ChannelWidget::color() const
{
	return m_color;
}

void ChannelWidget::setColor(QColor color)
{
	QString boxStyleSheet = QString(
		"QCheckBox#box::indicator {"
		"border: 2px solid %1;"
		"}"
		"QCheckBox#box::indicator:checked {"
		"background-color: %1;"
		"}"
	).arg(color.name());
	m_ui->box->setStyleSheet(boxStyleSheet);

	QString lineStyleSheet = QString(
		"QFrame#line[selected=true] {"
		"border: 2px solid %1;"
		"}"
	).arg(color.name());
	m_ui->line->setStyleSheet(lineStyleSheet);

	m_color = color;
}

QString ChannelWidget::fullName() const
{
	return m_fullName;
}

void ChannelWidget::setFullName(const QString& name)
{
	m_fullName = name;
}

QString ChannelWidget::shortName() const
{
	return m_shortName;
}

void ChannelWidget::setShortName(const QString &name)
{
	m_shortName = name;
}

void ChannelWidget::on_box_toggled(bool checked)
{
	if (checked) {
		if (!m_simplified)
			m_ui->name->setEnabled(true);
		m_ui->btn->setEnabled(true);
		// When enabling ChannelWidget select it as well
		if (!m_simplified)
			m_ui->name->setChecked(true);

	} else {
		// Unselect the ChannelWidget when it's about to be disabled
		if (!m_simplified) {
			QButtonGroup *group = m_ui->name->group();
			bool exclusive;
			if (group) {
				exclusive = group->exclusive();
				if (m_ui->name->isChecked()) {
					group->setExclusive(false);
				}
			}
			m_ui->name->setChecked(false);
			if (group) {
				group->setExclusive(exclusive);
			}
		}

		// Uncheck the menu button when disabling the ChannelWidget
		m_ui->btn->setChecked(false);

		if (!m_simplified)
			m_ui->name->setEnabled(false);

		m_ui->btn->setEnabled(false);
	}

	Q_EMIT enabled(checked);
}

void ChannelWidget::on_name_toggled(bool checked)
{
	Q_EMIT selected(checked);
}

void ChannelWidget::on_btn_toggled(bool checked)
{
	Q_EMIT menuToggled(checked);
}

void ChannelWidget::on_delBtn_clicked()
{
	setButtonNoGroup(m_ui->box);
	setButtonNoGroup(m_ui->name);
	setButtonNoGroup(m_ui->btn);

	Q_EMIT deleteClicked();
}

void ChannelWidget::setButtonNoGroup(QAbstractButton *btn)
{
	QButtonGroup *group = btn->group();
	if (group) {
		group->removeButton(btn);
	}
}