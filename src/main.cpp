#include <Geode/Geode.hpp>
#include <Geode/modify/SecretLayer5.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/utils/string.hpp>

using namespace cocos2d;

GJRewardObject* parse() {
	auto type = geode::Mod::get()->getSettingValue<std::string>("reward-type");
	auto amount = geode::Mod::get()->getSettingValue<int64_t>("reward-amount");
	auto safe = geode::Mod::get()->getSettingValue<bool>("safe-mode");
	auto gsm = GameStatsManager::sharedState();

	if (type == "Orbs") {
		if (!safe) gsm->incrementStat("14", amount);
		return GJRewardObject::create(SpecialRewardItem::Orbs, amount, 1);
	} else if (type == "Keys") {
		if (!safe) gsm->incrementStat("21", amount);
		return GJRewardObject::create(SpecialRewardItem::BonusKey, amount, 1);
	} else if (type == "Diamonds") {
		if (!safe) gsm->incrementStat("13", amount);
		return GJRewardObject::create(SpecialRewardItem::Diamonds, amount, 1);
	} else if (type == "Gold Keys") {
		if (!safe) gsm->incrementStat("43", amount);
		return GJRewardObject::create(SpecialRewardItem::GoldKey, amount, 1);
	}
	
	return nullptr;
};

class $modify(CustomWraithCodes, SecretLayer5) {
	struct Fields {
		std::string text = "";
	};

	bool init() {
		if (!SecretLayer5::init()) return false;

		if (!geode::Mod::get()->getSavedValue("disclaimer", false)) {
			auto popup = FLAlertLayer::create("Custom Wraith Codes", "This mod uses an <cy>anticheat bypass</c> which can get you leaderboard <cr>BANNED</c>. Safe mode is on by default. Safe mode can be disabled in the mod settings. Proceed with caution.", "OK");
			popup->m_scene = this;
			popup->show();
			geode::Mod::get()->setSavedValue("disclaimer", true);
		}

		return true;
	}

	void onSubmit(CCObject* sender) {
		m_fields->text = geode::utils::string::toLower(this->m_textInput->getString());
		SecretLayer5::onSubmit(sender);
	}

	void showSuccessWrapper() {
		this->showSuccessAnimation();
	}

	void fadeInLoadingWrapper() {
		this->m_circleSprite->fadeInCircle(false, 0.5f, 0.f);
	}

	void showRewardWrapper() {
		auto rewardObj = parse();
		
		if (rewardObj) {
			auto item = GJRewardItem::createWithObject(GJRewardType::Unknown, rewardObj);
			auto layer = RewardUnlockLayer::create(1, nullptr);

			auto scene = CCDirector::sharedDirector()->getRunningScene();
			scene->addChild(layer);
			layer->showCollectReward(item);

			auto call = CCCallFunc::create(this, nullptr);
			scene->runAction(CCSequence::create(CCDelayTime::create(2), call, nullptr));
		} else {
			FLAlertLayer::create("Error", "There was an error processing your reward.", "OK")->show();
		}
	}

	void onlineRewardStatusFailed() {
		auto code = geode::utils::string::toLower(geode::Mod::get()->getSettingValue<std::string>("code"));
		
		if (m_fields->text == code) {
			auto sequence = CCSequence::create(
				CCDelayTime::create(4),
				CCCallFunc::create(this, callfunc_selector(CustomWraithCodes::showSuccessWrapper)),
				CCDelayTime::create(1),
				CCCallFunc::create(this, callfunc_selector(CustomWraithCodes::fadeInLoadingWrapper)),
				CCCallFunc::create(this, callfunc_selector(CustomWraithCodes::showRewardWrapper)),
				nullptr
			);

			CCDirector::sharedDirector()->getRunningScene()->runAction(sequence);

			this->m_wraithButton->setEnabled(true);
		} else {
			SecretLayer5::onlineRewardStatusFailed();
		}
	}
};