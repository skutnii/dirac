/*
 * App.hpp
 *
 *  Created on: Jan 6, 2023
 *      Author: skutnii
 */

#ifndef SRC_APP_HPP_
#define SRC_APP_HPP_

#include <optional>
#include <string>

namespace dirac {

class App {
public:
	App(int argc, char **argv);
	App() = delete;
	App(const App& other) = delete;
	App(App&& other) = delete;
	App& operator=(const App& other) = delete;

	~App() = default;

	void setVar(const std::string& name, const std::string& value);

	int run();
	int compute(const std::string& expr) const;
private:
	int runShell();

	bool _useFloat = false;
	std::optional<size_t> _lineTerms;
	std::string _commandLineExpr;
};

} /* namespace dirac */

#endif /* SRC_APP_HPP_ */
