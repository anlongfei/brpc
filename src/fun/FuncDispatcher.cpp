#pragma once
#include <algorithm>
#include "src/fun/FuncDispatcher.h"
#include "src/util/RpcException.h"


namespace brpc{

template<class T>
static bool listHas(const List<T>& lst, const T& ele)
{
	return std::find(lst.begin(), lst.end(), ele) != lst.end();
}

FuncDispatcher::~FuncDispatcher()
{
	clearAllFunction();
	clearAllVar();
}

void FuncDispatcher::unregisterFunction(cstring funcName)
{
	auto itor = m_funcMap.find(funcName);
	if (itor == m_funcMap.end()) {
		return;
	}

	FuncList& funcList = itor->second;

	for(auto itor = funcList.begin(); itor != funcList.end(); ++itor) {
		delete (*itor);
	}
	m_funcMap.erase(funcName);
}

void FuncDispatcher::clearAllFunction()
{
	for (auto itor = m_funcMap.begin(); itor != m_funcMap.end(); /*++itor*/) {
		FuncList& funcList = itor->second;

		for(auto itor2 = funcList.begin(); itor2 != funcList.end(); ++itor2) {
			delete (*itor2);
		}
		itor = m_funcMap.erase(itor);
	}

	DispatcherList& ed = m_extendDispatchers;
	for (auto itor = ed.begin(); itor != ed.end(); ++itor) {
		FuncDispatcher* dispatcher = (*itor);
		if(dispatcher) {
			dispatcher->clearAllFunction();
		}
	}
}

AnyFunction* FuncDispatcher::matchedFunction(cstring name, const ObjectList& args)
{
	checkNullPtr(name);
	FuncList matchedList;
	FuncList allList= findFunction(name);
	float max = 1;//-1
	//find all matched  functions
	for (unsigned int i = 0; i < allList.size(); i++) {
		AnyFunction* func = allList[i];
		checkNullPtr(func);
		float m = func->matchArgsType(args);//return 0~1.0 (1.0: all matched)
		if (m > max){
			max = m;
			matchedList.clear();
			matchedList.push_back(func);
		}
		else if (m == max){
			matchedList.push_back(func);
		}
	}

	//find most matched function
	const int size = matchedList.size();
	if (size > 1){
		StringBuilder msg(size * 20);
		msg.append(String::format("functions(%d): [", size));
		for (unsigned int i = 0; i < matchedList.size(); i++) {
			AnyFunction* func = matchedList[i];
			checkNullPtr(func);
			if(i > 0)
				msg.append(", ");
			msg.append(func->toString());
		}
		msg.append("]");
		throwpe(AmbiguityFunctionException(msg.toString()));
	}
	else if (size == 1) {
		return matchedList[0];
	}
	else{
		return null;
	}
}


//matched all Function include Extend
AnyFunction* FuncDispatcher::matchedAllFunction(cstring name,
	const ObjectList& args)
{
	AnyFunction* fun = matchedFunction(name, args);

	if (fun != null){
		return fun;
	}
	else{
		DispatcherList& ed = m_extendDispatchers;
		for (auto itor = ed.begin(); itor != ed.end(); ++itor) {
			FuncDispatcher* dispatcher = (*itor);
			fun = dispatcher->matchedAllFunction(name, args);
			if (fun != null)
				break;
		}

		return fun;
	}
}

AnyFunction* FuncDispatcher::getFunctionFromAll(cstring name, const ObjectList& args)
{
	AnyFunction* func = this->matchedAllFunction(name, args);
	if (func == null){
		String strArgs = args.toString();
		strArgs = strArgs.substring(1, strArgs.length()-2);
		throwpe(ArgNotMatchedException(String::format("No function matched: %s(%s)",
			name, strArgs.c_str())));
	}
	return func;
}

Object* FuncDispatcher::call(cstring name, const ObjectList& args)
{
	checkNullPtr(name);
	String fname = name;
	if (fname.contain(".") && !hasFunction(name)) {
		int pos = fname.rfind(".");
		String fthis = fname.substring(0, pos);
		fname = fname.substring(pos+1);
		return call(fthis, fname, args);
	}
	else {
		AnyFunction* func = getFunctionFromAll(name, args);
		return AnyFunction::invoke(func, args);
	}
}

Object* FuncDispatcher::call(cstring obj, cstring name, const ObjectList& args)
{
	checkNullPtr(obj);
	checkNullPtr(name);
	if(strlen(obj) == 0)
		return call(name, args);
	//NOTE: don't use const_cast which is a dangerous operation
	/*ObjectList& argsWithThis = const_cast<ObjectList&>(args);*/
	ObjectList argsWithThis;
	argsWithThis.lease(args); // argsWithEvent will not be auto deleted!
	ScopePointer<ObjectRef> thisObj = new ObjectRef(obj, this);
	(void)argsWithThis.insert(0, thisObj);//this
	AnyFunction* func = getFunctionFromAll(name, argsWithThis);
	Object* rs = AnyFunction::invoke(func, argsWithThis);
	return rs;
}

bool FuncDispatcher::addFunction(AnyFunction* func)
{
	checkNullPtr(func);
	BRpcUtil::debug(":> %s\n", func->toString().c_str());
	auto& funcList = m_funcMap[func->name()];
	for(auto itor = funcList.begin(); itor != funcList.end(); ++itor) {
		if(func->operator==(**itor))
			return false;
	}
	funcList.push_back(func);
	return true;
}

List<String> FuncDispatcher::listFunctions() const
{
	List<String> funcs;
	for (auto itor = m_funcMap.begin(); itor != m_funcMap.end(); ++itor) {
		String name = itor->first;
		if(!listHas(funcs, name))
			funcs.push_back(name);
	}

	return funcs;
}

bool FuncDispatcher::hasFunction(cstring name) const
{
	auto itor = m_funcMap.find(name);
	if (itor != m_funcMap.end())
		return true;
	return false;
}

FuncDispatcher::FuncList FuncDispatcher::findFunction(cstring name) const
{
	auto itor = m_funcMap.find(name);
	if (itor == m_funcMap.end()) {
		//throwpe(NotFoundException(name));
		return FuncList();
	}
	return itor->second;
}

bool FuncDispatcher::registerVar(cstring name, Object* var)
{
	return m_objMap.insert(std::make_pair(name, var)).second;
}

bool FuncDispatcher::unregisterVar(cstring name)
{
	return m_objMap.erase(name) > 0;
}

Object* FuncDispatcher::getVar(cstring name) const
{
	auto itor = m_objMap.find(name);
	if (itor == m_objMap.end()) {
		//throwpe(NotFoundException(name));
		return null;
	}
	return itor->second;
}

List<String> FuncDispatcher::listVars() const
{
	List<String> vars;
	for(auto itor = m_objMap.begin(); itor != m_objMap.end(); ++itor) {
		vars.push_back(itor->first);
	}

	return vars;
}

Object* FuncDispatcher::getVarFromAll(cstring name) const
{
	auto itor = m_objMap.find(name);
	if(itor == m_objMap.end()) {
		//throwpe(NotFoundException(name));
		Object* var = null;
		auto& ed = m_extendDispatchers;
		for (auto i = ed.begin(); i != ed.end(); ++i) {
			FuncDispatcher* dispatcher = (*i);
			var = dispatcher->getVarFromAll(name);
			if (var != null)
				break;
		}

		return var;
	}
	return itor->second;
}

void FuncDispatcher::clearAllVar()
{
	for(auto itor = m_objMap.begin(); itor != m_objMap.end(); /*++itor*/) {
		delete itor->second;
		itor = m_objMap.erase(itor);
	}

	DispatcherList& ed = m_extendDispatchers;
	for (auto itor = ed.begin(); itor != ed.end(); ++itor) {
		FuncDispatcher* dispatcher = (*itor);
		dispatcher->clearAllVar();
	}
}

String FuncDispatcher::functionsAsString() const
{
	StringBuilder sb(m_objMap.size() * 20);
	for(auto itor = m_funcMap.begin(); itor != m_funcMap.end(); ++itor) {
		const FuncList& funcList = itor->second;

		for(auto itor2 = funcList.begin(); itor2 != funcList.end(); ++itor2) {
			auto& func = *itor2;
			if(func) {
				sb.append(func->toString());
				sb.append("\n");
			}
		}
	}

	return sb.toString();
}

String FuncDispatcher::varsAsString() const
{
	StringBuilder sb(m_objMap.size() * 20);
	for(auto itor = m_objMap.begin(); itor != m_objMap.end(); ++itor) {
		sb.append(itor->first);
		sb.append("\n");
	}

	return sb.toString();
}

String FuncDispatcher::toString() const
{
	return this->name();
}

bool FuncDispatcher::extend(FuncDispatcher* dispatcher)
{
	checkNullPtr(dispatcher);

	auto itor = std::find(m_extendDispatchers.begin(),
		m_extendDispatchers.end(), dispatcher);
	if(itor == m_extendDispatchers.end()){
		m_extendDispatchers.push_back(dispatcher);
		return true;
	}
	return false;
}

bool FuncDispatcher::exclude(FuncDispatcher* dispatcher)
{
	auto itor = std::find(m_extendDispatchers.begin(),
		m_extendDispatchers.end(), dispatcher);
	if(itor != m_extendDispatchers.end()){
		m_extendDispatchers.erase(itor);
		return true;
	}
	return false;
}

}//end of namespace brpc