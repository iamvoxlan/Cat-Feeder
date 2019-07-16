package com.example.catfeeder.model

class Status {
    var statusMorning : Boolean
    var statusNoon : Boolean
    var statusNight : Boolean

    constructor(){
        statusMorning = false
        statusNoon = false
        statusNight = false
    }
    constructor(statusMorning: Boolean, statusNoon: Boolean, statusNight: Boolean) {
        this.statusMorning = statusMorning
        this.statusNoon = statusNoon
        this.statusNight = statusNight
    }
}