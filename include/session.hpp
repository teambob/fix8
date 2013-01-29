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
#ifndef _FIX8_SESSION_HPP_
#define _FIX8_SESSION_HPP_

#include <Poco/Net/StreamSocket.h>
#include <tbb/atomic.h>

//-------------------------------------------------------------------------------------------------
namespace FIX8 {

//-------------------------------------------------------------------------------------------------
/// Quickfix style sessionid.
class SessionID
{
	static RegExp _sid;

	begin_string _beginString;
	sender_comp_id _senderCompID;
	target_comp_id _targetCompID;

	f8String _id, _rid;

public:
	/*! Ctor.
	    \param beginString Fix begin string
	    \param senderCompID Fix senderCompID string
	    \param targetCompID Fix targetCompID string */
	SessionID(const f8String& beginString, const f8String& senderCompID, const f8String& targetCompID)
		: _beginString(beginString), _senderCompID(senderCompID), _targetCompID(targetCompID) { make_id(); }

	/*! Ctor.
	    \param beginString Fix begin string field
	    \param senderCompID Fix senderCompID string field
	    \param targetCompID Fix targetCompID string field */
	SessionID(const begin_string& beginString, const sender_comp_id& senderCompID, const target_comp_id& targetCompID)
		: _beginString(beginString), _senderCompID(senderCompID), _targetCompID(targetCompID) { make_id(); }

	/*! Ctor.
	    \param from SessionID string */
	SessionID(const f8String& from) { from_string(from); }

	/*! Ctor.
	    \param from SessionID field */
	SessionID(const SessionID& from) : _beginString(from._beginString), _senderCompID(from._senderCompID),
		_targetCompID(from._targetCompID), _id(from._id) {}

	SessionID() {}

	/// Dtor.
	virtual ~SessionID() {}

	/*! Create a sessionid and reverse sessionid strings. */
	void make_id();

	/// Create a sessionid string.
	void from_string(const f8String& from);

	/*! Get the beginstring field.
	    \return beginstring */
	const begin_string& get_beginString() const { return _beginString; }

	/*! Get the sender_comp_id field.
	    \return sender_comp_id */
	const sender_comp_id& get_senderCompID() const { return _senderCompID; }

	/*! Get the target_comp_id field.
	    \return target_comp_id */
	const target_comp_id& get_targetCompID() const { return _targetCompID; }

	/*! Get the target_comp_id field.
	    \return target_comp_id */
	const f8String& get_id() const { return _id; }

	/*! Targetcompid equivalence operator.
	    \return true if both Targetcompids are the same */
	bool same_sender_comp_id(const target_comp_id& targetCompID) const { return targetCompID() == _senderCompID(); }

	/*! Sendercompid equivalence operator.
	    \return true if both Sendercompids are the same */
	bool same_target_comp_id(const sender_comp_id& senderCompID) const { return senderCompID() == _targetCompID(); }

	/*! Inserter friend.
	    \param os stream to send to
	    \param what SessionID
	    \return stream */
	friend std::ostream& operator<<(std::ostream& os, const SessionID& what) { return os << what._id; }
};

//-------------------------------------------------------------------------------------------------
/// Session states and semantics.
struct States
{
	enum Tests
	{
		pr_begin_str, pr_logged_in, pr_low, pr_high, pr_comp_id, pr_target_id, pr_logon_timeout, pr_resend,
	};

	enum SessionStates
	{
		st_continuous, st_session_terminated,
		st_wait_for_logon, st_not_logged_in, st_logon_sent, st_logon_received, st_logoff_sent, st_logoff_received,
		st_test_request_sent, st_sequence_reset_sent, st_sequence_reset_received,
		st_resend_request_sent, st_resend_request_received
	};

	static bool is_established(const SessionStates& ss)
		{ return ss != st_wait_for_logon && ss != st_not_logged_in && ss != st_logon_sent; }
};

//-------------------------------------------------------------------------------------------------
class Persister;
class Logger;
class Connection;

//-------------------------------------------------------------------------------------------------
struct LoginParameters
{
	enum { default_retry_interval=5000, default_login_retries=100 };

	LoginParameters() : _login_retry_interval(default_retry_interval), _login_retries(default_login_retries),
		_reset_sequence_numbers(), _recv_buf_sz(), _send_buf_sz() {}

	LoginParameters(const unsigned login_retry_interval, const unsigned login_retries,
		const default_appl_ver_id& davi, const bool reset_seqnum=false, unsigned recv_buf_sz=0, unsigned send_buf_sz=0)
		: _login_retry_interval(login_retry_interval), _login_retries(login_retries),
		_reset_sequence_numbers(reset_seqnum), _davi(davi), _recv_buf_sz(recv_buf_sz), _send_buf_sz(send_buf_sz) {}

	LoginParameters(const LoginParameters& from)
		: _login_retry_interval(from._login_retry_interval), _login_retries(from._login_retries),
		_reset_sequence_numbers(from._reset_sequence_numbers), _davi(from._davi),
		_recv_buf_sz(from._recv_buf_sz), _send_buf_sz(from._send_buf_sz) {}

	LoginParameters& operator=(const LoginParameters& that)
	{
		if (this != &that)
		{
			_login_retry_interval = that._login_retry_interval;
			_login_retries = that._login_retries;
			_reset_sequence_numbers = that._reset_sequence_numbers;
			_davi = that._davi;
			_recv_buf_sz = that._recv_buf_sz;
			_send_buf_sz = that._send_buf_sz;
		}
		return *this;
	}

	unsigned _login_retry_interval, _login_retries;
	bool _reset_sequence_numbers;
	default_appl_ver_id _davi;
	unsigned _recv_buf_sz, _send_buf_sz;
};

//-------------------------------------------------------------------------------------------------
/// Fix8 Base Session. User sessions derive from this class.
class Session
{
	static RegExp _seq;

public:
	enum SessionControl { shutdown, print, debug, count };
	typedef ebitset_r<SessionControl> Control;

private:
	typedef StaticTable<const f8String, bool (Session::*)(const unsigned, const Message *)> Handlers;
	Handlers _handlers;

	/*! Initialise atomic members.
	  \param st the initial session state */
	void atomic_init(States::SessionStates st);

protected:
	Control _control;
	tbb::atomic<unsigned> _next_send_seq, _next_receive_seq;
	tbb::atomic<States::SessionStates> _state;
	Tickval _last_sent, _last_received;
	const F8MetaCntx& _ctx;
	Connection *_connection;
	unsigned _req_next_send_seq, _req_next_receive_seq;
	SessionID _sid;

	LoginParameters _loginParamaters;

	Persister *_persist;
	Logger *_logger, *_plogger;

	Timer<Session> _timer;
	TimerEvent<Session> _hb_processor;

	/// Heartbeat generation service thread.
	bool heartbeat_service();

	/*! Logon callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_logon(const unsigned seqnum, const Message *msg);

	/*! Generate a logon message.
	    \param heartbeat_interval heartbeat interval
	    \param davi default appl version id (FIXT)
	    \return new Message */
	virtual Message *generate_logon(const unsigned heartbeat_interval, const f8String davi=f8String());

	/*! Logout callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_logout(const unsigned seqnum, const Message *msg);

	/*! Generate a logout message.
	    \return new Message */
	virtual Message *generate_logout();

	/*! Heartbeat callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_heartbeat(const unsigned seqnum, const Message *msg);

	/*! Generate a heartbeat message.
	    \param testReqID test request id
	    \return new Message */
	virtual Message *generate_heartbeat(const f8String& testReqID);

	/*! Resend request callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_resend_request(const unsigned seqnum, const Message *msg);

	/*! Generate a resend request message.
	    \param begin begin sequence number
	    \param end sequence number
	    \return new Message */
	virtual Message *generate_resend_request(const unsigned begin, const unsigned end=0);

	/*! Sequence reset callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_sequence_reset(const unsigned seqnum, const Message *msg);

	/*! Generate a sequence reset message.
	    \param newseqnum new sequence number
	    \param gapfillflag gap fill flag
	    \return new Message */
	virtual Message *generate_sequence_reset(const unsigned newseqnum, const bool gapfillflag=false);

	/*! Test request callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_test_request(const unsigned seqnum, const Message *msg);

	/*! Generate a test request message.
	    \param testReqID test request id
	    \return new Message */
	virtual Message *generate_test_request(const f8String& testReqID);

	/*! Reject callback.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_reject(const unsigned seqnum, const Message *msg) { return false; }

	/*! Generate a reject message.
	    \param seqnum message sequence number
	    \param what rejection text
	    \return new Message */
	virtual Message *generate_reject(const unsigned seqnum, const char *what);

	/*! Administrative message callback. Called on receipt of all admin messages.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_admin(const unsigned seqnum, const Message *msg) { return true; }

	/*! Application message callback. Called on receipt of all non-admin messages.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	virtual bool handle_application(const unsigned seqnum, const Message *msg);

	/*! Permit modification of message just prior to sending.
	     \param msg Message */
	virtual void modify_outbound(Message *msg) {}

	/*! Call user defined authentication with logon message.
	    \param id Session id of inbound connection
	    \param msg Message
	    \return true on success */
	virtual bool authenticate(SessionID& id, const Message *msg) { return true; }

	/// Recover next expected and next to send sequence numbers from persitence layer.
	virtual void recover_seqnums();

	/*! Create a new Fix message from metadata layer.
	    \param msg_type message type string
	    \return new Message */
	Message *create_msg(const f8String& msg_type)
	{
		const BaseMsgEntry *bme(_ctx._bme.find_ptr(msg_type));
		if (!bme)
			throw InvalidMetadata(msg_type);
		return bme->_create();
	}

public:
	/*! Ctor. Initiator.
	    \param ctx reference to generated metadata
	    \param sid sessionid of connecting session
		 \param persist persister for this session
		 \param logger logger for this session
		 \param plogger protocol logger for this session */
	Session(const F8MetaCntx& ctx, const SessionID& sid, Persister *persist=0, Logger *logger=0, Logger *plogger=0);

	/*! Ctor. Acceptor.
	    \param ctx reference to generated metadata
		 \param persist persister for this session
		 \param logger logger for this session
		 \param plogger protocol logger for this session */
	Session(const F8MetaCntx& ctx, Persister *persist=0, Logger *logger=0, Logger *plogger=0);

	/// Dtor.
	virtual ~Session();

	/*! Start the session.
	    \param connection established connection
	    \param wait if true, thread will wait till session ends before returning
	    \param send_seqnum if supplied, override the send login sequence number, set next send to seqnum+1
	    \param recv_seqnum if supplied, override the receive login sequence number, set next recv to seqnum+1
	    \param davi default appl version id (FIXT)
	    \return -1 on error, 0 on success */
	int start(Connection *connection, bool wait=true, const unsigned send_seqnum=0,
		const unsigned recv_seqnum=0, const f8String davi=f8String());

	/*! Process inbound messages. Called by connection object.
	    \param from raw fix message
	    \return true on success */
	virtual bool process(const f8String& from);

	/// Provides context to your retrans handler.
	struct RetransmissionContext
	{
		const unsigned _begin, _end, _interrupted_seqnum;
		unsigned _last;
		bool _no_more_records;

		RetransmissionContext(const unsigned begin, const unsigned end, const unsigned interrupted_seqnum)
			: _begin(begin), _end(end), _interrupted_seqnum(interrupted_seqnum), _last(), _no_more_records() {}

		friend std::ostream& operator<<(std::ostream& os, const RetransmissionContext& what)
		{
			os << "end:" << what._end << " last:" << what._last << " interrupted seqnum:"
				<< what._interrupted_seqnum << " no_more_records:" << std::boolalpha << what._no_more_records;
			return os;
		}
	};

	typedef std::pair<const unsigned, const f8String> SequencePair;

	/*! Retransmission callback. Called by framework with each message to be resent.
	    \param with pair of sequence number and raw fix message
	    \param rctx retransmission context
	    \return true on success */
	virtual bool retrans_callback(const SequencePair& with, RetransmissionContext& rctx);

	/*! Send message.
	    \param msg Message
	    \param custom_seqnum override sequence number with this value
	    \param no_increment if true, don't increment the seqnum after sending
	    \return true on success */
	virtual bool send(Message *msg, const unsigned custom_seqnum=0, const bool no_increment=false);
#if defined MSGRECYCLING

	/*! Send message and wait till no longer in use.
	    \param msg Message
	    \param custom_seqnum override sequence number with this value
	    \param waitval time to sleep(ms) before rechecking inuse
	    \return true on success */
	virtual bool send_wait(Message *msg, const unsigned custom_seqnum=0, const int waitval=10);
#endif

	/*! Process message (encode) and send.
	    \param msg Message
	    \return true on success */
	bool send_process(Message *msg);

	/// stop the session.
	void stop();

	/*! Get the connection object.
	    \return the connection object */
	Connection *get_connection() { return _connection; }

	/*! Get the metadata context object.
	    \return the context object */
	const F8MetaCntx& get_ctx() const { return _ctx; }

	/*! Log a message to the session logger.
	    \param what string to log
	    \param value optional value for the logger to use
	    \return true on success */
	bool log(const std::string& what, const unsigned value=0) const { return _logger ? _logger->send(what, value) : false; }

	/*! Log a message to the protocol logger.
	    \param what Fix message (string) to log
	    \param direction 0=out, 1=in
	    \return true on success */
	bool plog(const std::string& what, const unsigned direction=0) const { return _plogger ? _plogger->send(what, direction) : false; }

	/*! Return the last received timstamp
	    \return Tickval on success */
	const Tickval& get_last_received() const { return _last_received; }

	/*! Return the last sent timstamp
	    \return Tickval on success */
	const Tickval& get_last_sent() const { return _last_sent; }

	/// Update the last sent time.
	void update_sent() { _last_sent.now(); }

	/// Update the last received time.
	void update_received() { _last_received.now(); }

	/*! Check that a message has the correct sender/target compid for this session. Throws BadCompidId on error.
	    \param seqnum message sequence number
	    \param msg Message */
	void compid_check(const unsigned seqnum, const Message *msg);

	/*! Check that a message is in the correct sequence for this session. Will generated resend request if required. Throws InvalidMsgSequence, MissingMandatoryField, BadSendingTime.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true on success */
	bool sequence_check(const unsigned seqnum, const Message *msg);

	/*! Enforce session semantics. Checks compids, sequence numbers.
	    \param seqnum message sequence number
	    \param msg Message
	    \return true if message FAILS enforce ruless */
	bool enforce(const unsigned seqnum, const Message *msg);

	/*! Get the session id for this session.
	    \return the session id */
	const SessionID& get_sid() const { return _sid; }

	/*! Get the next sender sequence number
	    \return next sender sequence number */
	unsigned get_next_send_seq() const { return _next_send_seq; }

	/*! Set the LoginParameters
	    \param loginParamaters to populate from */
	void set_login_parameters(const LoginParameters& loginParamaters) { _loginParamaters = loginParamaters; }

	/*! Get the LoginParameters
	    \param loginParamaters to populate */
	void get_login_parameters(LoginParameters& loginParamaters) const { loginParamaters = _loginParamaters; }

	/*! Get the LoginParameters
	    \return loginParamaters */
	const LoginParameters& get_login_parameters() const { return  _loginParamaters; }

	/*! Set the persister.
	    \param pst pointer to persister object  */
	void set_persister(Persister *pst) { _persist = pst; }

	/*! Get the control object for this session.
	    \return the control object */
	Control& control() { return _control; }

	/*! See if this session is being shutdown.
	    \return true if shutdown is underway */
	bool is_shutdown() { return _control.has(shutdown); }

	friend struct StaticTable<const f8String, bool (Session::*)(const unsigned, const Message *)>;
};

//-------------------------------------------------------------------------------------------------

} // FIX8

#endif // _FIX8_SESSION_HPP_
