#pragma once

//------IO-------//
enum msgType{
  MSG_COMMAND,
  MSG_SUCCESS,
  MSG_ERROR,
  MSG_TEXT,
  MSG_BAD_REQUEST,
};

enum cmdType{
  CMD_LOGIN,
  CMD_SIGNUP,
  CMD_ACTION,
  CMD_PICKUP,
};
