#include<stdio.h>
#include<lua.h>
#include<lauxlib.h>
#include<lualib.h>

/// <summary>
/// 第一个例子
/// </summary>
void lua_example_first(lua_State* L) {
	
	char buff[256];
	int error;

	while (fgets(buff, sizeof(buff), stdin) != NULL)
	{
		error = luaL_loadbuffer(L, buff, strlen(buff), "line") || lua_pcall(L, 0, 0, 0);

		if (error)
		{
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1); /*pop error message from the stack*/
		}
	}
}

/// <summary>
/// 加载第一个lua脚本
/// </summary>
/// <param name=""></param>
void lua_example_dofile(lua_State* L) {
	luaL_dofile(L, "./scripts/firstLuaScript.lua");
}

/// <summary>
/// 得到一个全局变量
/// </summary>
/// <param name="L"></param>
void lua_example_getvar(lua_State* L) {
	
	luaL_dostring(L,"some_var = 500");
	lua_getglobal(L,"some_var");
	lua_Number some_var_in_c = lua_tonumber(L,-1);

	printf("some_var 在C侧的值是 %d\n",(int)some_var_in_c);
}

/// <summary>
/// Lua的栈操作
/// </summary>
/// <param name="L"></param>
void lua_example_stack(lua_State* L) {
	lua_settop(L, 0); // 清空栈，确保初始为空

	lua_pushnumber(L,300);  //stack[1] 或者 stack[-3]
	lua_pushnumber(L,450);  //stack[2] 或者 stack[-2]
	lua_pushnumber(L,438);  //stack[3] 或者 stack[-1]

	lua_Number element;
	element = lua_tonumber(L,-1);
	printf("最后增加的元素，位置在栈的索引3处的值为 %d\n",(int)element);

	// 栈变化过程：
    // 移除索引 2 后，栈中元素变为 [300, 438]
    // 索引 1: 300（原索引 1）
    // 索引 2: 438（原索引 3 下移填补空缺）
	lua_remove(L,2); //移除栈中索引为2的值438
	element = lua_tonumber(L,2);
	printf("当前栈索引为2处的值为 %d\n", (int)element);
}

/// <summary>
/// C侧调用lua函数
/// </summary>
/// <param name="L"></param>
void lua_example_call_lua_function(lua_State* L) {
	luaL_dofile(L,"./scripts/luaFunction.lua");
	lua_getglobal(L,"luafunction");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, 5); //将第一个参数压栈
		lua_pushnumber(L, 6); //将第二个参数压栈

		const int NUM_ARGS = 2; //参数的数量
		const int NUM_RETURNS = 1; //返回值的数量

		lua_pcall(L,NUM_ARGS,NUM_RETURNS,0);

		lua_Number luaResult = lua_tonumber(L,-1);

		printf("执行求和的lua函数的结果为：%f\n",(float)luaResult);
	}
}

/// <summary>
/// 检查处理错误
/// </summary>
/// <param name="L"></param>
void lua_example_check_handle_error(lua_State* L) {
	if (luaL_dostring(L, "some_var = 500...") != LUA_OK) {
		luaL_error(L,"Error: %s\n",lua_tostring(L,-1));
	}
	else {
		//执行正确逻辑
	}
}


int native_cfunction(lua_State* L) {
	lua_Number b = lua_tonumber(L, -1); //得到第二个参数b
	lua_Number a = lua_tonumber(L, -2); //得到第一个参数a
	lua_Number result = a + b;
	lua_pushnumber(L, result);
	return 1;  //返回函数有多少个返回值作为结果返回虚拟栈中
}
/// <summary>
/// Lua侧调用C函数
/// </summary>
/// <param name="L"></param>
void lua_example_call_c_function(lua_State* L) {
	lua_pushcfunction(L, native_cfunction); // 将 C 函数压入栈顶
	lua_setglobal(L,"native_cfunction"); // 将栈顶的值（native_cfunction）设置为 Lua 的全局变量 native_cfunction。操作后，栈被清空：[]。

	luaL_dofile(L,"./scripts/cFunction.lua");
	lua_getglobal(L,"cfunction");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L,3); //将3压栈，作为第一个参数
		lua_pushnumber(L,5); //将5压栈，作为第二个参数

		const int NUM_ARGS = 2; //参数的数量
		const int NUM_RETURNS = 1; //返回值的数量

		lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0);

		lua_Number luaResult = lua_tonumber(L, -1);

		printf("最终计算的结果 cfunction(3,5) = %f\n", (float)luaResult);
	}
}


typedef struct reacangle2d {
	int x;
	int y;
	int width;
	int height;
} rectangle;

/// <summary>
/// C侧发送数据到lua
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int create_rectangle(lua_State* L) {
	rectangle* rect = (rectangle*)lua_newuserdata(L,sizeof(rectangle));
	rect->x = 0;
	rect->y = 0;
	rect->width = 0;
	rect->height = 0;
	return 1; //表示返回一个类型作为userdata可供lua使用
}

int change_rectangle_size(lua_State* L) {
	rectangle* rect = (rectangle*)lua_touserdata(L,-3);
	rect->width = (int)lua_tonumber(L,-2);
	rect->height = (int)lua_tonumber(L,-1);
	return 0; //表示没有参数返回
}

void lua_example_userdata(lua_State* L) {

	//设置发送函数
	lua_pushcfunction(L,create_rectangle);
	lua_setglobal(L,"create_rectangle");

	//设置接收函数
	lua_pushcfunction(L, change_rectangle_size);
	lua_setglobal(L, "change_rectangle_size");

	luaL_dofile(L, "./scripts/rectangle.lua");

	lua_getglobal(L,"square");
	if (lua_isuserdata(L, -1)) {
		rectangle* r = (rectangle*)lua_touserdata(L, -1);
		printf("从lua侧得到一个rectangle值，width: %d, height: %d ",r->width,r->height);
	}
	else {
		printf("从lua侧没有获取到rectangle的值");
	}
}


void lua_example_table(lua_State* L) {
	lua_settop(L, 0); // 清空栈，确保初始为空
	if (luaL_dofile(L, "./scripts/configtable.lua") == LUA_OK) {
		lua_getglobal(L,"config_table");
		if (lua_istable(L, -1)) {
			lua_getfield(L,-1,"window_width");
			printf("在lua侧table中定义的 window_width的值为 %s\n",lua_tostring(L,-1));
			
			/*
			lua_getfield 将字段值压入栈顶后，原 table 的索引从 -1 变为 -2。此时 lua_setfield 的 table 索引参数 应为 -2，而非 -1。
			lua_setfield 需要将栈顶的值作为新字段值，所以需要提前将一个值压栈。
			*/
			// 方法 1：将 window_width 的值赋给 window_height
			//lua_setfield(L, -2, "window_height"); // 复用栈顶值

			// 方法 2：设置自定义值
			lua_pushnumber(L, 600);    
			
			//栈顶将600压栈之后table索引变为-3
			// 压入新值
			lua_setfield(L, -3, "window_height"); // 赋给 table
			//lua_setfield之后栈顶的 600 被弹出
			/*栈中的值分布如下：table索引从-3变为-2
			[-1]: window_width 的值
            [-2]: table
			*/
			lua_getfield(L, -2, "window_height");
			printf("在lua侧table中定义的 window_height的修改后的值为 %s\n", lua_tostring(L, -1));
			lua_pop(L, 1); // 清理栈顶的临时值
		}
	}
	else
	{
		luaL_error(L,"Error: %s\n",lua_tostring(L,-1));
	}
}

int main()
{
	//创建Lua虚拟机
	lua_State* L = luaL_newstate();    /*opens lua*/
	luaopen_base(L);   /*opens the basic library*/
	luaopen_table(L);  /*opens the table library*/
	luaopen_io(L);     /*opens the I/O library*/
	luaopen_string(L); /*opens the string lib.*/
	luaopen_math(L);   /*opens the math lib.*/

	/*****************具体函数执行*******************/
	//lua_example_first(L);
	//lua_example_dofile(L);
	//lua_example_getvar(L);
	//lua_example_stack(L);
	//lua_example_call_lua_function(L);
	//lua_example_check_handle_error(L);
	//lua_example_call_c_function(L);
	//lua_example_userdata(L);
	lua_example_table(L);
	/***********************************/

	//清理Lua虚拟机
	lua_close(L);

	return 0;
}