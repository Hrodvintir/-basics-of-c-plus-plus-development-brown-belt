#pragma once
#include <ostream>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>


struct StatsAggregator
{
	virtual ~StatsAggregator() {}

	virtual void Process(int value) = 0;
	virtual void PrintValue(std::ostream& out) const = 0;
};


namespace StatsAggregators
{
	class Sum : public StatsAggregator
    {
	public:
		void Process(int value) override;
		void PrintValue(std::ostream& out) const override;

	private:
		int sum = 0;
	};


	class Min : public StatsAggregator
    {
	public:
		void Process(int value) override;
		void PrintValue(std::ostream& out) const override;

	private:
		// Ðàíåå ìû íå ðàññìàòðèâàëè øàáëîí std::optional. Î í¸ì ìîæíî ïî÷èòàòü â äîêóìåíòàöèè
		// https://en.cppreference.com/w/cpp/utility/optional. Êðîìå òîãî, åìó áóäåò óäåëåíî âíèìàíèå
		// â ðàçäåëå ïðî ôóíêöèè
		std::optional<int> current_min;
	};


	class Max : public StatsAggregator 
    {
	public:
		void Process(int value) override;
		void PrintValue(std::ostream& out) const override;

	private:
		std::optional<int> current_max;
	};


	class Average : public StatsAggregator
    {
	public:
		void Process(int value) override;
		void PrintValue(std::ostream& out) const override;

	private:
		int sum = 0;
		int total = 0;
	};


	class Mode : public StatsAggregator
    {
	public:
		void Process(int value) override;
		void PrintValue(std::ostream& out) const override;

	private:
		std::unordered_map<int, int> count;
		std::optional<int> mode;
	};


	class Composite : public StatsAggregator
    {
	public:
		void Process(int value) override;
		void PrintValue(std::ostream& output) const override;

		void Add(std::unique_ptr<StatsAggregator> aggr);

	private:
		std::vector<std::unique_ptr<StatsAggregator>> aggregators;
	};


	void TestSum();
	void TestMin();
	void TestMax();
	void TestAverage();
	void TestMode();
	void TestComposite();
}
