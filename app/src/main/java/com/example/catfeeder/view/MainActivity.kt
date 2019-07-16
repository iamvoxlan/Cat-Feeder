package com.example.catfeeder.view

import android.app.AlertDialog
import android.app.Dialog
import android.app.ProgressDialog
import android.os.Build
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast
import com.example.catfeeder.R
import com.example.catfeeder.model.RealtimeDB
import com.example.catfeeder.model.Status
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener
import java.text.DateFormat
import java.text.SimpleDateFormat
import java.time.LocalDate
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter
import java.util.*

class MainActivity : AppCompatActivity(){

    private lateinit var buttonFeed : Button
    private lateinit var textTime :TextView
    private lateinit var textStatusMorning : TextView
    private lateinit var textStatusNoon : TextView
    private lateinit var textStatusNight : TextView
    private lateinit var status : Status

    val dataBase = FirebaseDatabase.getInstance().getReference()
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        buttonFeed = findViewById(R.id.buttonFeed)
        textTime = findViewById(R.id.textDate)
        textStatusMorning = findViewById(R.id.textMorningStatus)
        textStatusNoon =  findViewById(R.id.textNoonStatus)
        textStatusNight = findViewById(R.id.textNightStatus)

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val date =  LocalDateTime.now()
            val formatter = DateTimeFormatter.ofPattern("dd.MM.yyyy")
            var now: String =  date.format(formatter)
            textTime.setText("Date: "+now)
        } else {
            var date = Date()
            val formatter = SimpleDateFormat("dd MMM yyyy")
            val now: String = formatter.format(date)
            textTime.setText("Date: "+now)
        }



        textTime
        status = Status()
        getData()


        val  builder = AlertDialog.Builder(this)
        val dialogView = layoutInflater.inflate(R.layout.loading_bar, null)
        builder.setView(dialogView)
        builder.setCancelable(false)
        val dialog = builder.create()
        dialog.show()

        Handler().postDelayed({dialog.dismiss()}, 3000)


    }

    fun feedCat(view: View){
        val database = RealtimeDB()
        database.clickFeed()
        Toast.makeText(this, "You've feed the cat! Thank you :)", Toast.LENGTH_SHORT).show()
    }

    fun getStatus(status: Boolean) : String{
        var displayText = "Not Yet"
        if (status){
            displayText = "Done"
        }
        return displayText
    }

    fun getData() {
        dataBase.child("Status").addValueEventListener(object: ValueEventListener{
            override fun onCancelled(p0: DatabaseError) {
                Log.d("get data", p0.details)
            }

            override fun onDataChange(p0: DataSnapshot) {
                var newStatus = Status(
                    p0.child("statusMorning").getValue(Boolean::class.java)!!,
                    p0.child("statusNoon").getValue(Boolean::class.java)!!,
                    p0.child("statusNight").getValue(Boolean::class.java)!!
                )
                updateData(newStatus)
            }
        })
    }

    fun updateData(newStatus: Status){
        textStatusMorning.setText(getStatus(newStatus.statusMorning))
        textStatusNoon.setText(getStatus(newStatus.statusNoon))
        textStatusNight.setText(getStatus(newStatus.statusNight))
    }

}
