//-------------------------------------------------------------------------------------------------
#if 0

Fix8 is released under the GNU LESSER GENERAL PUBLIC LICENSE Version 3.

Fix8 Open Source FIX Engine.
Copyright (C) 2010-13 David L. Dight <fix@fix8.org>

Fix8 is free software: you can  redistribute it and / or modify  it under the  terms of the
GNU Lesser General  Public License as  published  by the Free  Software Foundation,  either
version 3 of the License, or (at your option) any later version.

Fix8 is distributed in the hope  that it will be useful, but WITHOUT ANY WARRANTY;  without
even the  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

You should  have received a copy of the GNU Lesser General Public  License along with Fix8.
If not, see <http://www.gnu.org/licenses/>.

BECAUSE THE PROGRAM IS  LICENSED FREE OF  CHARGE, THERE IS NO  WARRANTY FOR THE PROGRAM, TO
THE EXTENT  PERMITTED  BY  APPLICABLE  LAW.  EXCEPT WHEN  OTHERWISE  STATED IN  WRITING THE
COPYRIGHT HOLDERS AND/OR OTHER PARTIES  PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED   OR   IMPLIED,  INCLUDING,  BUT   NOT  LIMITED   TO,  THE  IMPLIED
WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO
THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

IN NO EVENT UNLESS REQUIRED  BY APPLICABLE LAW  OR AGREED TO IN  WRITING WILL ANY COPYRIGHT
HOLDER, OR  ANY OTHER PARTY  WHO MAY MODIFY  AND/OR REDISTRIBUTE  THE PROGRAM AS  PERMITTED
ABOVE,  BE  LIABLE  TO  YOU  FOR  DAMAGES,  INCLUDING  ANY  GENERAL, SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT
NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR
THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH
HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#endif
//-------------------------------------------------------------------------------------------------
#ifndef _FIX8_MYFIX_HPP_
#define _FIX8_MYFIX_HPP_

//-----------------------------------------------------------------------------------------
class myfix_session_client;

/// Example client message router. Derives from fix8 generated router class.
/*! Your application must define a class similar to this in order to receive
    the appropriate callback when Message::process is called. */
class tex_router_client : public FIX8::TEX::Myfix_Router
{
	myfix_session_client& _session;

public:
	/*! Ctor.
	    \param session client session */
	tex_router_client(myfix_session_client& session) : _session(session) {}

	/*! Execution report handler. Here is where you provide your own methods for the messages you wish to
		 handle. Only those messages that are of interest to you need to be implemented.
	    \param msg Execution report message session */
	virtual bool operator() (const FIX8::TEX::ExecutionReport *msg) const;
};

/// Example client session. Derives from FIX8::Session.
/*! Your application must define a class similar to this in order to create and connect a client.
    You must also implement handle_application in order to receive application messages from the framework. */
class myfix_session_client : public FIX8::Session
{
	tex_router_client _router;

public:
	/*! Ctor. Initiator.
	    \param ctx reference to generated metadata
	    \param sid sessionid of connecting session
		 \param persist persister for this session
		 \param logger logger for this session
		 \param plogger protocol logger for this session */
	myfix_session_client(const FIX8::F8MetaCntx& ctx, const FIX8::SessionID& sid, FIX8::Persister *persist=0,
		FIX8::Logger *logger=0, FIX8::Logger *plogger=0) : Session(ctx, sid, persist, logger, plogger), _router(*this) {}

	/*! Application message callback.
	    This method is called by the framework when an application message has been received and decoded. You
	  	 should implement this method and call the supplied Message::process.
	    \param seqnum Fix sequence number of the message
		 \param msg Mesage decoded (base ptr)
		 \return true on success */
	bool handle_application(const unsigned seqnum, const FIX8::Message *msg);

#if defined MSGRECYCLING
	//int preload_nos(const std::string& sym, TEX::Side side, TEX::OrdType ordtype, TEX::TimeInForce tif);
#endif
};

//-----------------------------------------------------------------------------------------
class myfix_session_server;

/// Example server message router. Derives from fix8 generated router class.
/*! Your application must define a class similar to this in order to receive
    the appropriate callback when Message::process is called. */
class tex_router_server : public FIX8::TEX::Myfix_Router
{
	myfix_session_server& _session;

public:
	/*! Ctor.
	    \param session server session */
	tex_router_server(myfix_session_server& session) : _session(session) {}

	/*! NewOrderSingle message handler. Here is where you provide your own methods for the messages you wish to
		 handle. Only those messages that are of interest to you need to be implemented.
	    \param msg NewOrderSingle message */
	virtual bool operator() (const FIX8::TEX::NewOrderSingle *msg) const;
};

/// Example server session. Derives from FIX8::Session.
/*! Your application must define a class similar to this in order to receive client connections.
    You must also implement handle_application in order to receive application messages from the framework. */
class myfix_session_server : public FIX8::Session
{
	tex_router_server _router;

public:
	/*! Ctor. Acceptor.
	    \param ctx reference to generated metadata
		 \param persist persister for this session
		 \param logger logger for this session
		 \param plogger protocol logger for this session */
	myfix_session_server(const FIX8::F8MetaCntx& ctx, FIX8::Persister *persist=0,
		FIX8::Logger *logger=0, FIX8::Logger *plogger=0) : Session(ctx, persist, logger, plogger),
		_router(*this) {}

	/*! Application message callback. This method is called by the framework when an application message has been received and decoded.
	    You should implement this method and call the supplied Message::process.
	    \param seqnum Fix sequence number of the message
		 \param msg Mesage decoded (base ptr)
		 \return true on success */
	bool handle_application(const unsigned seqnum, const FIX8::Message *msg);
};

//-------------------------------------------------------------------------------------------------
/// Simple menu system that will work with most term types.
class MyMenu
{
	FIX8::tty_save_state _tty;
	FIX8::MsgList _lst;
	FIX8::ConsoleMenu *_cm;

	/// Individual menu item.
	struct MenuItem
	{
		const char _key;
		const std::string _help;

		MenuItem(const char key, const std::string& help) : _key(key), _help(help) {}
		MenuItem() : _key(), _help() {}
		bool operator() (const MenuItem& a, const MenuItem& b) const { return a._key < b._key; }
	};

	myfix_session_client& _session;
	std::istream _istr;
	std::ostream& _ostr;

	typedef FIX8::StaticTable<const MenuItem, bool (MyMenu::*)(), MenuItem> Handlers;
	Handlers _handlers;

public:
	MyMenu(myfix_session_client& session, int infd, std::ostream& ostr, FIX8::ConsoleMenu *cm=0)
		: _tty(infd), _cm(cm), _session(session), _istr(new FIX8::fdinbuf(infd)), _ostr(ostr) {}
	virtual ~MyMenu() {}

	std::istream& get_istr() { return _istr; }
	std::ostream& get_ostr() { return _ostr; }
	bool process(const char ch) { return (this->*_handlers.find_ref(MenuItem(ch, std::string())))(); }

	bool new_order_single();
	bool new_order_single_50();
	bool new_order_single_1000();
	bool help();
	bool nothing() { return true; }
	bool do_exit() { return false; }
	bool do_logout();
	bool create_msgs();
	bool edit_msgs();
	bool delete_msgs();
	bool send_msgs();
	bool write_msgs();
	bool read_msgs();

	FIX8::tty_save_state& get_tty() { return _tty; }

	friend struct FIX8::StaticTable<const MenuItem, bool (MyMenu::*)(), MenuItem>;
};

//-----------------------------------------------------------------------------------------
/// A random number generator wrapper.
struct RandDev
{
	static void init()
	{
		time_t tval(time(0));
		srandom (static_cast<unsigned>(((tval % getpid()) * tval)));
	}

	template<typename T>
   static T getrandom(const T range=0)
   {
		T target(random());
		return range ? target / (RAND_MAX / range + 1) : target;
	}
};

#endif // _FIX8_MYFIX_HPP_

