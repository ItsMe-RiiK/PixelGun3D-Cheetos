#include "currency.h"
#include "../../ui/menu.h"

namespace CurrencyMod
{
  void InitMenu()
  {
    Menu::AddMenuItem({"-- CURRENCY --", Menu::ItemType::Header});

    // Lottery
    Menu::AddMenuItem({"Enable Lottery Modifier", Menu::ItemType::Bool, &Settings::bEnableLotteryModifier});
    Menu::AddMenuItem({"Modify Chest Output", Menu::ItemType::Bool, &Settings::bModifyChestOutput});
    Menu::AddMenuItem(
      {"Chest Output Amount", Menu::ItemType::Int, nullptr, nullptr, &Settings::iChestOutputAmount, 0.0f, 0.0f, 0.0f,
       -99999, 99999, 10}
    );

    // Match Reward
    Menu::AddMenuItem({"Enable Reward Multiplier", Menu::ItemType::Bool, &Settings::bEnableRewardMultiplier});
    Menu::AddMenuItem(
      {"Coins Multiplier", Menu::ItemType::Float, nullptr, &Settings::fCoinsMultiplier, 1.0f, 10000.0f, 1.0f}
    );

    // PixelPass
    Menu::AddMenuItem({"Spoof PixelPass Premium", Menu::ItemType::Bool, &Settings::bSpoofPixelPassPremium});
  }
}  // namespace CurrencyMod
