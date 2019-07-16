package com.example.catfeeder.model

import com.google.firebase.database.FirebaseDatabase

class RealtimeDB {
    val ref = FirebaseDatabase.getInstance().getReference()

    fun clickFeed(){
        ref.child("buttonPress").setValue(true)
    }


}