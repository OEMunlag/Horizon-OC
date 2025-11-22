#include "value_choice_gui.h"
#include "../format.h"
#include "fatal_gui.h"
#include <sstream>
#include <iomanip>

ValueChoiceGui::ValueChoiceGui(std::uint32_t selectedValue,
                               const ValueRange& range,
                               const std::string& categoryName,
                               ValueChoiceListener listener,
                               const ValueThresholds& thresholds,
                               bool enableThresholds,
                               std::map<std::uint32_t, std::string> labels)
    : selectedValue(selectedValue),
      range(range),
      categoryName(categoryName),
      listener(listener),
      thresholds(thresholds),
      enableThresholds(enableThresholds),
      labels(labels)
{
}

ValueChoiceGui::~ValueChoiceGui()
{
}

std::string ValueChoiceGui::formatValue(std::uint32_t value)
{
    std::ostringstream oss;

    if (value == 0) {
        return VALUE_DEFAULT_TEXT;
    }

    double displayValue = static_cast<double>(value) / static_cast<double>(range.divisor);

    oss << std::fixed << std::setprecision(range.decimalPlaces) << displayValue;

    if (!range.suffix.empty()) {
        oss << " " << range.suffix;
    }
    return oss.str();
}

int ValueChoiceGui::getSafetyLevel(std::uint32_t value)
{
    if (value > thresholds.danger) {
        return 2;
    }
    if (value > thresholds.warning) {
        return 1;
    }
    return 0;
}

tsl::elm::ListItem* ValueChoiceGui::createValueListItem(std::uint32_t value, bool selected, int safety)
{
    std::string text = formatValue(value);
    if (selected) {
        text += " \uE14B";
    }

    std::string rightText = "";
    auto it = labels.find(value);
    if (it != labels.end()) {
        rightText = it->second;
    }

    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(text, rightText, false);

    switch (safety)
    {
    case 0:
        listItem->setTextColor(tsl::Color(255, 255, 255, 255));
        listItem->setValueColor(tsl::Color(255, 255, 255, 255));
        break;
    case 1:
        listItem->setTextColor(tsl::Color(255, 165, 0, 255));
        listItem->setValueColor(tsl::Color(255, 165, 0, 255));
        break;
    case 2:
        listItem->setTextColor(tsl::Color(255, 0, 0, 255));
        listItem->setValueColor(tsl::Color(255, 0, 0, 255));
        break;
    }

    if (!rightText.empty())
        listItem->setValueColor(tsl::Color(180, 180, 180, 255));

    listItem->setClickListener([this, value](u64 keys)
    {
        if ((keys & HidNpadButton_A) == HidNpadButton_A && this->listener) {
            if (this->listener(value)) {
                tsl::goBack();
            }
            return true;
        }
        return false;
    });

    return listItem;
}

void ValueChoiceGui::listUI()
{
    if (!categoryName.empty()) {
        this->listElement->addItem(new tsl::elm::CategoryHeader(categoryName));
    }

    this->listElement->addItem(this->createValueListItem(0, this->selectedValue == 0, 0));

    for (std::uint32_t value = range.min; value <= range.max; value += range.step)
    {
        int safety = getSafetyLevel(value);
        bool selected = (value == this->selectedValue);
        this->listElement->addItem(this->createValueListItem(value, selected, safety));
    }

    this->listElement->jumpToItem("", "\uE14B");
}
