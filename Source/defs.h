/*
  ==============================================================================

    defs.h
    Created: 25 Sep 2022 10:56:52am
    Author:  Jonny McGregor

  ==============================================================================
*/

#pragma once

//All Commands at Tapedecks disposal
enum CommandIDs {
    loadSession = 1,
    saveSession,
    togglePropertiesWindow,

    createTrack,
    deleteSelectedTracks,

    togglePlayback,
    recordPlayback,

    updatePlayheadInModel,
};