#include "gwlib/gwlib.h"



#define SQL_INSERT_MT "INSERT INTO trafficMT (id,phoneNumber,shortNumber,receivedTime,deliveryTime,dispatchTime,deliveryCount,msgText,carrierMsgId,integratorMsgId,status,errCode,errText,serviceId,serviceName,carrierId,integratorId,integratorQueueId,connectionId) \
VALUES(NULL,'%S','%S',now(),now(),'0000-00-00 00:00:00',%s,'%S',%s,'%S','%s',%d,'%S',%S,'%S',%S,%S,%S,'%S')"

#define SQL_SELECT_MO "SELECT tm.id,tm.phoneNumber,tm.shortNumber,tm.receivedTime,tm.deliveryTime,tm.dispatchTime,tm.deliveryCount,tm.msgText,tm.status,tm.errCode,tm.errText,tm.serviceId,tm.serviceName,tm.carrierId,tm.integratorId,tm.integratorQueueId,s.moUrl,c.name \
FROM trafficMO tm LEFT JOIN carrier c ON (tm.carrierId=c.id) \
LEFT JOIN service s  ON (tm.serviceId=s.id) \
WHERE tm.status='QUEUED' \
AND tm.deliveryTime<=now() \
AND tm.integratorQueueId=%s \
LIMIT 1000 "

#define SQL_SELECT_SERVICE "SELECT id,serviceName,shortNumber,connectionId,carrierId,integratorId, integratorQueueId FROM service \
WHERE id=%s "

#define SQL_UPDATE_MO_STATUS "UPDATE trafficMO  \
SET status = '%s', \
	dispatchTime = now() \
WHERE id= %s "

#define SQL_UPDATE_MO_STATUS_ERROR "UPDATE trafficMO  \
SET status = '%s',errCode=%d,errText='%s', \
	dispatchTime = now() \
WHERE id= %s "

#define SQL_SELECT_CARRIER_ROUTE "SELECT c.id AS id,c.name AS name,cast(group_concat(p.preffix separator ';') as char(5000) charset utf8) AS preffix \
FROM carrier c, preffixMapping p \
WHERE c.id = p.carrierId \
GROUP BY c.name"

#define SQL_SELECT_PORTED_NUMBER "SELECT carrierId FROM ported \
where portedNumber='%s' "

#define SQL_SELECT_BLACK_LIST "SELECT * FROM blackList \
where phoneNumber='%s' "



#include "gw/msg.h"
#include <mysql/mysql.h>

MYSQL_RES* mysql_select(const Octstr *sql);
void mysql_update(const Octstr *sql);
void sql_save_msg(Msg *msg, Octstr *momt);
Msg *mysql_fetch_msg();
void sql_shutdown();
struct server_type *sqlbox_init_mysql(Cfg* cfg);
Octstr *sqlbox_id;

struct server_type {
	Octstr *type;
	void (*sql_enter)(Cfg *);
	void (*sql_leave)();
	Msg *(*sql_fetch_msg)();
	void (*sql_save_msg)(Msg *, Octstr *);
};

