/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INTERNETSERVICE_H
#define INTERNETSERVICE_H

#include <QObject>
#include <QList>
#include <QUrl>

#include "core/song.h"
#include "playlist/playlistitem.h"
#include "ui/settingsdialog.h"
#include "widgets/multiloadingindicator.h"

class InternetModel;
class LibraryFilterWidget;

class InternetService : public QObject {
  Q_OBJECT

public:
  // Constructs a new internet service with the given name and model. The name
  // should be user-friendly (like 'DigitallyImported' or 'Last.fm').
  InternetService(const QString& name, InternetModel* model, QObject* parent = NULL);
  virtual ~InternetService() {}

  QString name() const { return name_; }
  InternetModel* model() const { return model_; }

  virtual QStandardItem* CreateRootItem() = 0;
  virtual void LazyPopulate(QStandardItem* parent) = 0;

  virtual void ShowContextMenu(const QModelIndex& index, const QPoint& global_pos) {}
  virtual void ItemDoubleClicked(QStandardItem* item) {}
  // Give the service a chance to do a custom action when data is dropped on it
  virtual void DropMimeData(const QMimeData* data, const QModelIndex& index) {}

  virtual PlaylistItem::Options playlistitem_options() const { return PlaylistItem::Default; }
  // Redefine this function to add service' specific actions to the playlist item
  virtual QList<QAction*> playlistitem_actions(const Song& song) { return QList<QAction*>(); }

  virtual QWidget* HeaderWidget() const { return NULL; }

  virtual void ReloadSettings() {}

  virtual QString Icon() { return QString(); }

public slots:
  // Show configuration panel for services which have one
  virtual void ShowConfig() { }
  

signals:
  void StreamError(const QString& message);
  void StreamMetadataFound(const QUrl& original_url, const Song& song);
  void OpenSettingsAtPage(SettingsDialog::Page page);

  void AddToPlaylistSignal(QMimeData* data);

private slots:
  void AppendToPlaylist();
  void ReplacePlaylist();
  void OpenInNewPlaylist();

protected:
  // Subclass provides the currently selected QModelIndex on InternetService's
  // request.
  virtual QModelIndex GetCurrentIndex() = 0;

  // Returns all the playlist insertion related QActions (see below).
  QList<QAction*> GetPlaylistActions();

  // Returns the 'append to playlist' QAction.
  QAction* GetAppendToPlaylistAction();
  // Returns the 'replace playlist' QAction.
  QAction* GetReplacePlaylistAction();
  // Returns the 'open in new playlist' QAction.
  QAction* GetOpenInNewPlaylistAction();

  // Describes how songs should be added to playlist.
  enum AddMode {
    // appends songs to the current playlist
    AddMode_Append,
    // clears the current playlist and then appends all songs to it
    AddMode_Replace,
    // creates a new, empty playlist and then adds all songs to it
    AddMode_OpenInNew
  };

  // Adds the 'index' element to playlist using the 'add_mode' mode.
  void AddItemToPlaylist(const QModelIndex& index, AddMode add_mode);
  // Adds the 'indexes' elements to playlist using the 'add_mode' mode.
  void AddItemsToPlaylist(const QModelIndexList& indexes, AddMode add_mode);

private:
  InternetModel* model_;
  QString name_;

  QAction* append_to_playlist_;
  QAction* replace_playlist_;
  QAction* open_in_new_playlist_;
  QAction* separator_;
};

Q_DECLARE_METATYPE(InternetService*);

#endif // INTERNETSERVICE_H
