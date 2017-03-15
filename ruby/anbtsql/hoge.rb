require "anbt-sql-formatter/rule"
require "anbt-sql-formatter/formatter"



rule = AnbtSql::Rule.new
formatter = AnbtSql::Formatter.new(rule)


#print formatter.format("SELECT name, age FROM users WHERE age >= 20 and name like '田中%' order by name")

tn="xx"
timecond="xxx"
s=<<EOS
            SELECT
              user_id,
              SUM(stone_paid),
            FROM (
              SELECT
                user_id,
                JSON_EXTRACT(mki_request_result,"$.shop_category") AS category,
                IFNULL(INTEGER(JSON_EXTRACT(mki_request_result,"$.USER_PARAMETER_TYPE_STONE_PAID")),0) AS stone_paid,
                IFNULL(INTEGER(JSON_EXTRACT(mki_request_result,"$.USER_PARAMETER_TYPE_STONE_FREE")),0) AS stone_free,
              FROM
                #{tn}
              WHERE
                mki_logtype = "buy_product"
                #{timecond}
            )
            WHERE
              category = "1"
            GROUP BY
              user_id
EOS

print formatter.format(s)


