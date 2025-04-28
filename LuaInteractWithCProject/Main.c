#include<stdio.h>
#include<lua.h>
#include<lauxlib.h>
#include<lualib.h>

/// <summary>
/// ��һ������
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
/// ���ص�һ��lua�ű�
/// </summary>
/// <param name=""></param>
void lua_example_dofile(lua_State* L) {
	luaL_dofile(L, "./scripts/firstLuaScript.lua");
}

/// <summary>
/// �õ�һ��ȫ�ֱ���
/// </summary>
/// <param name="L"></param>
void lua_example_getvar(lua_State* L) {
	
	luaL_dostring(L,"some_var = 500");
	lua_getglobal(L,"some_var");
	lua_Number some_var_in_c = lua_tonumber(L,-1);

	printf("some_var ��C���ֵ�� %d\n",(int)some_var_in_c);
}

/// <summary>
/// Lua��ջ����
/// </summary>
/// <param name="L"></param>
void lua_example_stack(lua_State* L) {
	lua_settop(L, 0); // ���ջ��ȷ����ʼΪ��

	lua_pushnumber(L,300);  //stack[1] ���� stack[-3]
	lua_pushnumber(L,450);  //stack[2] ���� stack[-2]
	lua_pushnumber(L,438);  //stack[3] ���� stack[-1]

	lua_Number element;
	element = lua_tonumber(L,-1);
	printf("������ӵ�Ԫ�أ�λ����ջ������3����ֵΪ %d\n",(int)element);

	// ջ�仯���̣�
    // �Ƴ����� 2 ��ջ��Ԫ�ر�Ϊ [300, 438]
    // ���� 1: 300��ԭ���� 1��
    // ���� 2: 438��ԭ���� 3 �������ȱ��
	lua_remove(L,2); //�Ƴ�ջ������Ϊ2��ֵ438
	element = lua_tonumber(L,2);
	printf("��ǰջ����Ϊ2����ֵΪ %d\n", (int)element);
}

/// <summary>
/// C�����lua����
/// </summary>
/// <param name="L"></param>
void lua_example_call_lua_function(lua_State* L) {
	luaL_dofile(L,"./scripts/luaFunction.lua");
	lua_getglobal(L,"luafunction");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, 5); //����һ������ѹջ
		lua_pushnumber(L, 6); //���ڶ�������ѹջ

		const int NUM_ARGS = 2; //����������
		const int NUM_RETURNS = 1; //����ֵ������

		lua_pcall(L,NUM_ARGS,NUM_RETURNS,0);

		lua_Number luaResult = lua_tonumber(L,-1);

		printf("ִ����͵�lua�����Ľ��Ϊ��%f\n",(float)luaResult);
	}
}

/// <summary>
/// ��鴦�����
/// </summary>
/// <param name="L"></param>
void lua_example_check_handle_error(lua_State* L) {
	if (luaL_dostring(L, "some_var = 500...") != LUA_OK) {
		luaL_error(L,"Error: %s\n",lua_tostring(L,-1));
	}
	else {
		//ִ����ȷ�߼�
	}
}


int native_cfunction(lua_State* L) {
	lua_Number b = lua_tonumber(L, -1); //�õ��ڶ�������b
	lua_Number a = lua_tonumber(L, -2); //�õ���һ������a
	lua_Number result = a + b;
	lua_pushnumber(L, result);
	return 1;  //���غ����ж��ٸ�����ֵ��Ϊ�����������ջ��
}
/// <summary>
/// Lua�����C����
/// </summary>
/// <param name="L"></param>
void lua_example_call_c_function(lua_State* L) {
	lua_pushcfunction(L, native_cfunction); // �� C ����ѹ��ջ��
	lua_setglobal(L,"native_cfunction"); // ��ջ����ֵ��native_cfunction������Ϊ Lua ��ȫ�ֱ��� native_cfunction��������ջ����գ�[]��

	luaL_dofile(L,"./scripts/cFunction.lua");
	lua_getglobal(L,"cfunction");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L,3); //��3ѹջ����Ϊ��һ������
		lua_pushnumber(L,5); //��5ѹջ����Ϊ�ڶ�������

		const int NUM_ARGS = 2; //����������
		const int NUM_RETURNS = 1; //����ֵ������

		lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0);

		lua_Number luaResult = lua_tonumber(L, -1);

		printf("���ռ���Ľ�� cfunction(3,5) = %f\n", (float)luaResult);
	}
}


typedef struct reacangle2d {
	int x;
	int y;
	int width;
	int height;
} rectangle;

/// <summary>
/// C�෢�����ݵ�lua
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int create_rectangle(lua_State* L) {
	rectangle* rect = (rectangle*)lua_newuserdata(L,sizeof(rectangle));
	rect->x = 0;
	rect->y = 0;
	rect->width = 0;
	rect->height = 0;
	return 1; //��ʾ����һ��������Ϊuserdata�ɹ�luaʹ��
}

int change_rectangle_size(lua_State* L) {
	rectangle* rect = (rectangle*)lua_touserdata(L,-3);
	rect->width = (int)lua_tonumber(L,-2);
	rect->height = (int)lua_tonumber(L,-1);
	return 0; //��ʾû�в�������
}

void lua_example_userdata(lua_State* L) {

	//���÷��ͺ���
	lua_pushcfunction(L,create_rectangle);
	lua_setglobal(L,"create_rectangle");

	//���ý��պ���
	lua_pushcfunction(L, change_rectangle_size);
	lua_setglobal(L, "change_rectangle_size");

	luaL_dofile(L, "./scripts/rectangle.lua");

	lua_getglobal(L,"square");
	if (lua_isuserdata(L, -1)) {
		rectangle* r = (rectangle*)lua_touserdata(L, -1);
		printf("��lua��õ�һ��rectangleֵ��width: %d, height: %d ",r->width,r->height);
	}
	else {
		printf("��lua��û�л�ȡ��rectangle��ֵ");
	}
}


void lua_example_table(lua_State* L) {
	lua_settop(L, 0); // ���ջ��ȷ����ʼΪ��
	if (luaL_dofile(L, "./scripts/configtable.lua") == LUA_OK) {
		lua_getglobal(L,"config_table");
		if (lua_istable(L, -1)) {
			lua_getfield(L,-1,"window_width");
			printf("��lua��table�ж���� window_width��ֵΪ %s\n",lua_tostring(L,-1));
			
			/*
			lua_getfield ���ֶ�ֵѹ��ջ����ԭ table �������� -1 ��Ϊ -2����ʱ lua_setfield �� table �������� ӦΪ -2������ -1��
			lua_setfield ��Ҫ��ջ����ֵ��Ϊ���ֶ�ֵ��������Ҫ��ǰ��һ��ֵѹջ��
			*/
			// ���� 1���� window_width ��ֵ���� window_height
			//lua_setfield(L, -2, "window_height"); // ����ջ��ֵ

			// ���� 2�������Զ���ֵ
			lua_pushnumber(L, 600);    
			
			//ջ����600ѹջ֮��table������Ϊ-3
			// ѹ����ֵ
			lua_setfield(L, -3, "window_height"); // ���� table
			//lua_setfield֮��ջ���� 600 ������
			/*ջ�е�ֵ�ֲ����£�table������-3��Ϊ-2
			[-1]: window_width ��ֵ
            [-2]: table
			*/
			lua_getfield(L, -2, "window_height");
			printf("��lua��table�ж���� window_height���޸ĺ��ֵΪ %s\n", lua_tostring(L, -1));
			lua_pop(L, 1); // ����ջ������ʱֵ
		}
	}
	else
	{
		luaL_error(L,"Error: %s\n",lua_tostring(L,-1));
	}
}

int main()
{
	//����Lua�����
	lua_State* L = luaL_newstate();    /*opens lua*/
	luaopen_base(L);   /*opens the basic library*/
	luaopen_table(L);  /*opens the table library*/
	luaopen_io(L);     /*opens the I/O library*/
	luaopen_string(L); /*opens the string lib.*/
	luaopen_math(L);   /*opens the math lib.*/

	/*****************���庯��ִ��*******************/
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

	//����Lua�����
	lua_close(L);

	return 0;
}