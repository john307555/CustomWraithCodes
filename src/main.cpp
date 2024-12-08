#include <Geode/Geode.hpp>
#include <Geode/modify/SecretLayer5.hpp>
#include <Geode/modify/GameStatsManager.hpp>

using namespace cocos2d;

GJRewardObject* parse() {
	auto type = geode::Mod::get()->getSettingValue<std::string>("reward-type");
	auto amount = geode::Mod::get()->getSettingValue<int64_t>("reward-amount");
	auto gsm = GameStatsManager::sharedState();

	if (type == "Orbs") {
		gsm->incrementStat("14", amount);
		return GJRewardObject::create(SpecialRewardItem::Orbs, amount, 1);
	} else if (type == "Keys") {
		gsm->incrementStat("21", amount);
		return GJRewardObject::create(SpecialRewardItem::BonusKey, amount, 1);
	} else if (type == "Diamonds") {
		gsm->incrementStat("13", amount);
		return GJRewardObject::create(SpecialRewardItem::Diamonds, amount, 1);
	} else if (type == "Gold Keys") {
		gsm->incrementStat("43", amount);
		return GJRewardObject::create(SpecialRewardItem::GoldKey, amount, 1);
	}
	
	return nullptr;
};

class $modify(CustomWraithCodes, SecretLayer5) {
	struct Fields {
		std::string text = "";
	};

	void onSubmit(CCObject* sender) {
		m_fields->text = this->m_textInput->getString();
		std::transform(
			m_fields->text.begin(),
			m_fields->text.end(),
			m_fields->text.begin(),
			[](unsigned char c) { return std::tolower(c); }
		);
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
		auto code = geode::Mod::get()->getSettingValue<std::string>("code");
		std::transform(
			code.begin(),
			code.end(),
			code.begin(),
			[](unsigned char c) { return std::tolower(c); }
		);
		
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