#include <Geode/Geode.hpp>
#include <Geode/modify/SecretLayer5.hpp>
#include <regex>

using namespace cocos2d;

GJRewardObject* parse(const std::string& reward) {
	std::regex rewardRegex(R"((\d+)(orb|key|diamond))");
	std::smatch match;

	if (std::regex_match(reward, match, rewardRegex)) {
		int amount = std::stoi(match[1].str());
		std::string type = match[2].str();

		if (type == "orb") {
			return GJRewardObject::create(SpecialRewardItem::Orbs, amount, 1);
		} else if (type == "key") {
			return GJRewardObject::create(SpecialRewardItem::BonusKey, amount, 1);
		} else if (type == "diamond") {
			return GJRewardObject::create(SpecialRewardItem::Diamonds, amount, 1);
		}
	}

	return nullptr;
}

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
		auto rewardObj = parse(geode::Mod::get()->getSavedValue<std::string>("reward", "1orb"));
		
		if (rewardObj) {
			auto item = GJRewardItem::createWithObject(GJRewardType::Key100Treasure, rewardObj);
			auto layer = RewardUnlockLayer::create(2, nullptr);

			auto scene = CCDirector::sharedDirector()->getRunningScene();
			scene->addChild(layer);
			layer->showCollectReward(item);

			auto call = CCCallFunc::create(this, nullptr);
			scene->runAction(CCSequence::create(CCDelayTime::create(2), call, nullptr));
		} else {
			FLAlertLayer::create("Invalid Reward", "The reward string provided is invalid. See the mod page for help.", "OK")->show();
		}
	}

	void onlineRewardStatusFailed() {
		auto code = geode::Mod::get()->getSavedValue<std::string>("code", "kale");
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