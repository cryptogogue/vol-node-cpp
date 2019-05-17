#TODO: port this to javascript

import sys, requests

HOST = "http://localhost:" + sys.argv [ 1 ] + "/users/"

# TODO: clear existing data tables before populating, 
# test modifying existing info,
# test deleting user, test effects on connection,
# test manual connection removal

# Test function to populate users database with test users
def populate_accounts ():
    print ( "Adding 9 accounts to database..." )
    accounts = [
        {
            "phone": "1",
            "phone": "2",
            "phone": "3",
            "phone": "4",
            "phone": "5",
            "phone": "6",
            "phone": "7",
            "phone": "8",
            "phone": "9",
        }
    ]
    for account in accounts:
        r = requests.post ( HOST, data = account )

        if r.status_code != requests.codes.ok:
            return r.status_code
    
    print ( "Posts complete with no errors!" )

    r = requests.get ( HOST )
    print ( "TEST: Expecting 9 entries: " + str ( len ( r.json ())))

    return 200

# Test function to add user information to 5 users
def populate_users ():
    print ( "Adding user information to 5 accounts..." )
    users = [
        { "fname": "Steven", "lname": "Toast" },
        { "fname": "Ray", "lname": "Purchase", "email": "rayfpurchase@continental.co.uk" },
        { "fname": "Gary", "lname": "Schandling", "email": "something@something.com" },
        { "fname": "Patricia", "lname": "Harding" },
        { "fname": "Princess", "lname": "Carolyn", "email": "agent@email.com" }
    ]
    user_id_list = [ "1", "3", "5", "7", "9" ]
    user_id_list_index = 0

    for user in users:
        r = requests.put ( HOST + "/" + user_id_list[user_id_list_index], data=user )
        user_id_list_index += 1

        if r.status_code != requests.codes.ok:
            return r.status_code

    print ( "Puts complete with no errors!" )

    r = requests.get ( HOST )
    print ( "TEST: Expecting 9 entries: " + str ( len ( r.json ())))

    r = requests.get ( HOST + "/5" )
    r = r.json [ 0 ]
    print ( "TEST: Testing correct insertion of data: " + ( r.fname == "Gary" and r.lname == "Schandling" ))

    return 200

# Test function to add vendor information to 3 accounts
def populate_vendors ():
    print ( "Adding vendor information to 3 accounts..." )
    vendors = [
        {
            "vendor_name": "Johnny's House of Noodles", 
            "vendor_website": "notpenguinpublishing@realmail.com", 
            "vendor_location": "New York", 
            "vendor_type": "Front"
        },
        {
            "vendor_name": "Bananna Stand", 
            "vendor_website": "alwaysmoney@bs.com", 
            "vendor_location": "California", 
            "vendor_type": "Frozen Fruit"
        },
        {
            "vendor_name": "Scramble Studios", 
            "vendor_website": "onemoretake@va.co.uk", 
            "vendor_location": "London", 
            "vendor_type": "Recording Studio"
        }
    ]
    user_id_list = [ "1", "5", "7" ]
    user_id_list_index = 0

    for vendor in vendors:
        r = requests.post ( HOST + "/" + user_id_list[user_id_list_index], data=vendor )
        user_id_list_index += 1

        if r.status_code != requests.codes.ok:
            return r.status_code

    print ( "Puts complete with no errors!" )
    r = requests.get ( HOST + "/5" )
    r = r.json ()[ 0 ]
    print ( "TEST: Testing correct insertion of data: " + ( r.vendor_name == "Bananna Stand" and r.vendor_type == "Frozen Fruit" ))

    return 200

# Test function to add customer connections under vendors
def connect_users ():
    print ( "Connecting a few users..." )

    user_id_list = [ "1", "5", "7" ]

    for user_id in user_id_list:
        customer = { "customer_phone" }
        r = requests.post ( HOST + "/" + user_id + "/network", data=customer )


if __name__ == "__main__":

    print ( "Creating accounts... " + str ( populate_accounts ()))
    print ( "Creating users... " + str ( populate_users ()))
    print ( "Creating vendors... " + str ( populate_vendors ()))
    print ( "Connecting vendors to customers... " + str ( connect_users ()))
