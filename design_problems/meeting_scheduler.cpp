#include<bits/stdc++.h>
using namespace std;

class User{
    string name;
    string email;
    int id;
public:
    void notify(Notification notification);
};

class Interval{
    int startime, endtime;
};

class MeetingRoom{
    string id;
    int capacity;
};

class Meeting{
    enum class Status{
        CREATED,
        CONFIRMED,
        CANCELLED
    };
    int roomId;
    int organiserId;
    map<int, bool> participants;
    Interval interval;
    Status status;

public:
    Meeting(MeetingRoom room, int organiserId, Interval interval);
    void addParticipant(int userId);
    void updateParticipant(int userId);
};

class Scheduler{
    map<int, Meeting> meetings;
    map<int, set<Interval>> rooms;

public:
    // read path 
    bool checkStatus(int roomId, Interval interval);

    // read path
    vector<int> getAvailableRoom(Interval interval);

    // write path
    int createMeeting(Meeting meeting);

    // write path
    void cancelMeeting(int meetingId);
};

class Notification{
    string content;
    int id;
    int creationTime;
};

// observer 
class NotificationManger{
public:
    bool sendNotification(Notification notification, vector<User> users);
    bool cancelNotification(Notification notification, vector<User> users);
};

// singleton
class Calender{
    map<int, User> users;
    map<int, MeetingRoom> rooms;
    shared_ptr<Scheduler> scheduler;
    shared_ptr<NotificationManger> nmanager;

public:
    int addUser(User user);
    User getUser(int userId);

    int addRoom(MeetingRoom room);
    void removeRoom(MeetingRoom room);

    void createMeeting(MeetingRoom room, int organiserId, Interval interval, vector<int> participants);
    void createMeeting(int organiserId, Interval interval, vector<int> participants);
    void cancelMeeting(int meetingId);

};

int main(){


    return 0;
}